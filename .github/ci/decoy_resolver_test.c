#define main engsel_program_main
#include "../../src/engsel.c"
#undef main

#include <assert.h>

static Account test_account(const char *subscription_type){
	Account account;
	memset(&account,0,sizeof(account));
	account.subscription_type=(char *)subscription_type;
	return account;
}

static void reset_decoy_config(void){
	memset(cfg.decoy_prepaid_family_code,0,sizeof(cfg.decoy_prepaid_family_code));
	memset(cfg.decoy_prepaid_package_number,0,sizeof(cfg.decoy_prepaid_package_number));
	memset(cfg.decoy_prioritas_family_code,0,sizeof(cfg.decoy_prioritas_family_code));
	memset(cfg.decoy_prioritas_package_number,0,sizeof(cfg.decoy_prioritas_package_number));
	memset(cfg.decoy_priohybrid_family_code,0,sizeof(cfg.decoy_priohybrid_family_code));
	memset(cfg.decoy_priohybrid_package_number,0,sizeof(cfg.decoy_priohybrid_package_number));
}

static DecoyConfigChoice choice_for(const char *subscription_type,int expected_result){
	Account account=test_account(subscription_type);
	DecoyConfigChoice choice; memset(&choice,0,sizeof(choice));
	char *error=NULL;
	int result=decoy_config_choice(&account,&choice,&error);
	assert(result==expected_result);
	if(expected_result<0) assert(error&&*error);
	else assert(error==NULL);
	free(error);
	return choice;
}

static void test_subscription_mapping_and_fallback(void){
	reset_decoy_config();
	DecoyConfigChoice choice=choice_for("PREPAID",0);
	assert(!strcmp(choice.category,"PREPAID")&&!choice.configured);
	choice=choice_for("PRIORITAS",0);
	assert(!strcmp(choice.category,"PRIORITAS")&&!choice.prioritas_fallback);
	choice=choice_for("GO",0);
	assert(!strcmp(choice.category,"PRIORITAS")&&!choice.prioritas_fallback);
	choice=choice_for("PRIOHYBRID",0);
	assert(!strcmp(choice.category,"PRIORITAS")&&choice.prioritas_fallback);
	choice=choice_for("NONPRIORITY",0);
	assert(!strcmp(choice.category,"PREPAID"));

	snprintf(cfg.decoy_prepaid_family_code,sizeof(cfg.decoy_prepaid_family_code),"prepaid-family");
	snprintf(cfg.decoy_prepaid_package_number,sizeof(cfg.decoy_prepaid_package_number),"2");
	snprintf(cfg.decoy_prioritas_family_code,sizeof(cfg.decoy_prioritas_family_code),"prio-family");
	snprintf(cfg.decoy_prioritas_package_number,sizeof(cfg.decoy_prioritas_package_number),"3");
	snprintf(cfg.decoy_priohybrid_family_code,sizeof(cfg.decoy_priohybrid_family_code),"hybrid-family");
	snprintf(cfg.decoy_priohybrid_package_number,sizeof(cfg.decoy_priohybrid_package_number),"4");
	choice=choice_for("PREPAID",1);
	assert(!strcmp(choice.family_code,"prepaid-family")&&choice.package_number==2);
	choice=choice_for("PRIORITAS",1);
	assert(!strcmp(choice.family_code,"prio-family")&&choice.package_number==3);
	choice=choice_for("GO",1);
	assert(!strcmp(choice.family_code,"prio-family")&&choice.package_number==3);
	choice=choice_for("PRIOHYBRID",1);
	assert(!strcmp(choice.family_code,"hybrid-family")&&choice.package_number==4&&!choice.prioritas_fallback);

	cfg.decoy_priohybrid_family_code[0]=0;
	cfg.decoy_priohybrid_package_number[0]=0;
	choice=choice_for("PRIOHYBRID",1);
	assert(!strcmp(choice.family_code,"prio-family")&&choice.package_number==3&&choice.prioritas_fallback);

	cfg.decoy_priohybrid_family_code[0]=0;
	snprintf(cfg.decoy_priohybrid_package_number,sizeof(cfg.decoy_priohybrid_package_number),"1");
	choice_for("PRIOHYBRID",-1);
	cfg.decoy_priohybrid_package_number[0]=0;
	cfg.decoy_prioritas_package_number[0]=0;
	choice_for("PRIOHYBRID",-1);
	choice_for("GO",-1);

	reset_decoy_config();
	snprintf(cfg.decoy_prepaid_family_code,sizeof(cfg.decoy_prepaid_family_code),"not-a-uuid");
	snprintf(cfg.decoy_prepaid_package_number,sizeof(cfg.decoy_prepaid_package_number),"1");
	choice=choice_for("PREPAID",1);
	assert(!strcmp(choice.family_code,"not-a-uuid"));
	snprintf(cfg.decoy_prepaid_family_code,sizeof(cfg.decoy_prepaid_family_code),"invalid family");
	choice_for("PREPAID",-1);
	snprintf(cfg.decoy_prepaid_family_code,sizeof(cfg.decoy_prepaid_family_code),"valid-family");
	snprintf(cfg.decoy_prepaid_package_number,sizeof(cfg.decoy_prepaid_package_number),"0");
	choice_for("PREPAID",-1);
	snprintf(cfg.decoy_prepaid_package_number,sizeof(cfg.decoy_prepaid_package_number),"2147483648");
	choice_for("PREPAID",-1);
}

static void test_flattened_package_number_and_account_isolation(void){
	const char *family_response=
		"{\"status\":\"SUCCESS\",\"data\":{\"package_variants\":["
		"{\"package_options\":[{\"order\":1,\"package_option_code\":\"A1\"},{\"order\":1,\"package_option_code\":\"A2\"}]},"
		"{\"package_options\":[{\"order\":1,\"package_option_code\":\"B1\"}]}]}}";
	char *error=NULL; int total=0;
	char *code=flattened_family_option_code(family_response,1,&total,&error);
	assert(code&&!strcmp(code,"A1")&&total==3&&error==NULL); free(code);
	code=flattened_family_option_code(family_response,2,&total,&error);
	assert(code&&!strcmp(code,"A2")&&total==3&&error==NULL); free(code);
	code=flattened_family_option_code(family_response,3,&total,&error);
	assert(code&&!strcmp(code,"B1")&&total==3&&error==NULL); free(code);
	code=flattened_family_option_code(family_response,4,&total,&error);
	assert(code==NULL&&total==3&&error); free(error); error=NULL;

	const char *subscriber_a="{\"data\":{\"package_variants\":[{\"package_options\":[{\"package_option_code\":\"subscriber-A-option\"}]}]}}";
	const char *subscriber_b="{\"data\":{\"package_variants\":[{\"package_options\":[{\"package_option_code\":\"subscriber-B-option\"}]}]}}";
	char *code_a=flattened_family_option_code(subscriber_a,1,&total,&error);
	char *code_b=flattened_family_option_code(subscriber_b,1,&total,&error);
	assert(code_a&&code_b&&strcmp(code_a,code_b));
	assert(!strcmp(code_a,"subscriber-A-option")&&!strcmp(code_b,"subscriber-B-option"));
	free(code_a); free(code_b);
}

static void test_error_codes_and_utf8_literals(void){
	assert(api_error_151("{\"code\":\"151\",\"message\":\"GENERAL_ERROR_HANDLER\"}"));
	assert(api_error_151("{\"code\":151}"));
	assert(!api_error_151("{\"code\":150}"));
	assert(!strcmp(DECOY_V2_PAYMENT_FOR,"\xF0\x9F\xA4\xAB"));
	assert(!strcmp(FAMILY_LOOP_PAYMENT_FOR,"\xF0\x9F\xA4\x91"));
}

static void test_family_start_uses_api_order(void){
	QuotaList options={0}; size_t index=99;
	ql_add(&options,xstrdup("first"),xstrdup("variant-a"),xstrdup("code-a"),xstrdup("1"),xstrdup("variant-a-code"));
	ql_add(&options,xstrdup("second"),xstrdup("variant-a"),xstrdup("code-b"),xstrdup("2"),xstrdup("variant-a-code"));
	ql_add(&options,xstrdup("third"),xstrdup("variant-b"),xstrdup("code-c"),xstrdup("1"),xstrdup("variant-b-code"));
	assert(!family_purchase_start_index(&options,1,&index)&&index==0);
	assert(!family_purchase_start_index(&options,2,&index)&&index==1);
	assert(family_purchase_start_index(&options,3,&index));
	ql_free(&options);
}

int main(void){
	test_subscription_mapping_and_fallback();
	test_flattened_package_number_and_account_isolation();
	test_error_codes_and_utf8_literals();
	test_family_start_uses_api_order();
	puts("decoy resolver tests passed");
	return 0;
}

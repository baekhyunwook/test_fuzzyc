#include <string>
#include <fstream>
#include <streambuf>
#include <numeric>

#include "absl/status/status.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "antlr4-runtime.h"
#include "FunctionBaseListener.h"
#include "FunctionLexer.h"
#include "FunctionParser.h"
#include "ModuleBaseListener.h"
#include "ModuleLexer.h"
#include "ModuleParser.h"

namespace third_party {
namespace antlr4_grammars {
namespace fuzzyc {
namespace {

using ::antlr4::tree::ParseTreeWalker;
using ::fuzzyc_cc_function::FunctionBaseListener;
using ::fuzzyc_cc_function::FunctionLexer;
using ::fuzzyc_cc_function::FunctionParser;
using ::fuzzyc_cc_module::ModuleBaseListener;
using ::fuzzyc_cc_module::ModuleLexer;
using ::fuzzyc_cc_module::ModuleParser;
using ::testing::Test;

constexpr char kTestfilePath[] = "tests/testdata";

class FuzzycTest : public Test {
 protected:
  void SetUp() override {
    testfile_str_.open(kTestfilePath);
  }

  std::ifstream testfile_str_;
};

class TestModuleListener : public ModuleBaseListener {
 public:
  explicit TestModuleListener() {}

  ~TestModuleListener() override {}

  void enterFunction_def(ModuleParser::Function_defContext *ctx) override;

  void enterParameter_name(ModuleParser::Parameter_nameContext *ctx) override;

  void enterCompound_statement(
      ModuleParser::Compound_statementContext *ctx) override;

  std::vector<std::string> function_names;
  std::vector<std::string> parameter_names;
  std::vector<std::string> function_bodies;
};

void TestModuleListener::enterFunction_def(
    ModuleParser::Function_defContext *ctx) {
  function_names.push_back(ctx->function_name()->identifier()->getText());
}

void TestModuleListener::enterParameter_name(
    ModuleParser::Parameter_nameContext *ctx) {
  parameter_names.push_back(ctx->identifier()->getText());
}

void TestModuleListener::enterCompound_statement(
    ModuleParser::Compound_statementContext *ctx) {
  std::string body =
      ctx->getStart()->getInputStream()->getText(antlr4::misc::Interval(
          ctx->getStart()->getStartIndex(), ctx->getStop()->getStopIndex()));
  function_bodies.push_back(body);
}

class TestFunctionListner : public FunctionBaseListener {
 public:
  explicit TestFunctionListner() {}

  ~TestFunctionListner() override {}

  void enterDeclarator(FunctionParser::DeclaratorContext *ctx) override;

  void enterIdentifier(FunctionParser::IdentifierContext *ctx) override;

  void enterType_name(FunctionParser::Type_nameContext *ctx) override;

  std::set<std::string> declarators;
  std::set<std::string> identifiers;
  std::set<std::string> types;
};

void TestFunctionListner::enterDeclarator(
    FunctionParser::DeclaratorContext *ctx) {
  declarators.insert(ctx->getText());
}

void TestFunctionListner::enterIdentifier(
    FunctionParser::IdentifierContext *ctx) {
  identifiers.insert(ctx->getText());
}

void TestFunctionListner::enterType_name(
    FunctionParser::Type_nameContext *ctx) {
  types.insert(ctx->getText());
}

}  // namespace

TEST_F(FuzzycTest, FuzzycModule) {
  antlr4::ANTLRInputStream input_stream(testfile_str_);
  ModuleLexer module_lexer(&input_stream);
  antlr4::CommonTokenStream token_stream(&module_lexer);
  ModuleParser module_parser(&token_stream);
  TestModuleListener module_listener;
  ParseTreeWalker::DEFAULT.walk(&module_listener, module_parser.code());

  // Validate function names extracted by the fuzzy parser
  const std::vector<std::string> expected_function_names = {
      "parse_no_kvmapf", "parse_no_stealacc", "kvm_async_pf_task_wait",
      "paravirt_ops_setup"};
  EXPECT_THAT(module_listener.function_names, expected_function_names)
      << "Extracted function names do not match to the expectation";

  // Validate function parameters extracted by the fuzzy parser
  const std::vector<std::string> expected_parameter_names = {
      "arg", "arg", "token", "interrupt_kernel"};
  EXPECT_THAT(module_listener.parameter_names, expected_parameter_names)
      << "Extracted function names do not match to the expectation";

  // Validate function body extracted by the fuzzy parser
  const std::string expected_body = "kvmapf = 0;\n        return 0;";
  std::string found_function_bodies =
      std::accumulate(module_listener.function_bodies.begin(),
                      module_listener.function_bodies.end(), std::string());
  EXPECT_THAT(found_function_bodies, testing::HasSubstr(expected_body))
      << "Fail to find the expected function body.";
}

TEST_F(FuzzycTest, FuzzycFunction) {
  antlr4::ANTLRInputStream input_stream(testfile_str_);
  ModuleLexer module_lexer(&input_stream);
  antlr4::CommonTokenStream token_stream(&module_lexer);
  ModuleParser module_parser(&token_stream);
  TestModuleListener module_listener;
  ParseTreeWalker::DEFAULT.walk(&module_listener, module_parser.code());

  // Validate function information of kvm_async_pf_task_wait()
  {
    int index = std::find(module_listener.function_names.begin(),
                          module_listener.function_names.end(),
                          "kvm_async_pf_task_wait") -
                module_listener.function_names.begin();

    ASSERT_TRUE(index != module_listener.function_names.size());
    std::string test_function_body = module_listener.function_bodies.at(index);
    antlr4::ANTLRInputStream function_input_stream(test_function_body);
    FunctionLexer function_lexer(&function_input_stream);
    antlr4::CommonTokenStream function_token_stream(&function_lexer);
    FunctionParser function_parser(&function_token_stream);
    TestFunctionListner function_listener;
    ParseTreeWalker::DEFAULT.walk(&function_listener,
                                  function_parser.statements());

    std::set<std::string> expected_declarators({"*b", "n", "*e", "key"});
    EXPECT_THAT(function_listener.declarators, expected_declarators);
    std::set<std::string> expected_identifiers({"interrupt_kernel",
                                                "halted",
                                                "wq",
                                                "raw_spin_lock",
                                                "async_pf_sleepers",
                                                "n",
                                                "init_swait_queue_head",
                                                "lock",
                                                "rcu_irq_exit",
                                                "finish_swait",
                                                "current",
                                                "is_idle_task",
                                                "rcu_irq_enter",
                                                "prepare_to_swait_exclusive",
                                                "schedule",
                                                "b",
                                                "hash_32",
                                                "_find_apf_task",
                                                "native_safe_halt",
                                                "preempt_count",
                                                "hlist_del",
                                                "cpu",
                                                "IS_ENABLED",
                                                "link",
                                                "local_irq_enable",
                                                "hlist_add_head",
                                                "DECLARE_SWAITQUEUE",
                                                "KVM_TASK_SLEEP_HASHBITS",
                                                "key",
                                                "raw_spin_unlock",
                                                "e",
                                                "token",
                                                "kfree",
                                                "hlist_unhashed",
                                                "CONFIG_PREEMPT_COUNT",
                                                "local_irq_disable",
                                                "list",
                                                "TASK_UNINTERRUPTIBLE",
                                                "wait",
                                                "smp_processor_id",
                                                "rcu_preempt_depth"});
    EXPECT_THAT(function_listener.identifiers, expected_identifiers);
    std::set<std::string> expected_types(
        {"structkvm_task_sleep_node", "u32", "structkvm_task_sleep_head"});
    EXPECT_THAT(function_listener.types, expected_types);
  }

  // Validate function information of paravirt_ops_setup()
  {
    int index =
        std::find(module_listener.function_names.begin(),
                  module_listener.function_names.end(), "paravirt_ops_setup") -
        module_listener.function_names.begin();

    ASSERT_TRUE(index != module_listener.function_names.size());
    std::string test_function_body = module_listener.function_bodies.at(index);
    antlr4::ANTLRInputStream function_input_stream(test_function_body);
    FunctionLexer function_lexer(&function_input_stream);
    antlr4::CommonTokenStream function_token_stream(&function_lexer);
    FunctionParser function_parser(&function_token_stream);
    TestFunctionListner function_listener;
    ParseTreeWalker::DEFAULT.walk(&function_listener,
                                  function_parser.statements());

    std::set<std::string> expected_identifiers(
        {"cpu", "pv_ops", "name", "pv_info", "io_delay", "kvm_io_delay",
         "no_timer_check", "KVM_FEATURE_NOP_IO_DELAY", "kvm_para_has_feature"});
    EXPECT_THAT(function_listener.identifiers, expected_identifiers);
  }
  ASSERT_TRUE(true);
}

}  // namespace fuzzyc
}  // namespace antlr4_grammars
}  // namespace third_party

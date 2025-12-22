#include <gtest/gtest.h>
#include "json/json.h"

using namespace sid::json;

class FormatTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test data
        test_obj["name"] = "John";
        test_obj["age"] = 30;
        test_obj["active"] = true;
        
        test_arr.append(1);
        test_arr.append("hello");
        test_arr.append(false);
    }
    
    value test_obj;
    value test_arr;
};

TEST_F(FormatTest, CompactFormat) {
    std::string result = test_obj.to_str();
    
    // Should not contain extra whitespace
    EXPECT_EQ(result.find('\n'), std::string::npos);
    EXPECT_EQ(result.find("  "), std::string::npos);
    
    // Should contain all data
    EXPECT_NE(result.find("\"name\""), std::string::npos);
    EXPECT_NE(result.find("\"John\""), std::string::npos);
    EXPECT_NE(result.find("30"), std::string::npos);
}

TEST_F(FormatTest, PrettyFormat) {
    format fmt(format_type::pretty);
    std::string result = test_obj.to_str(fmt);
    
    // Should contain newlines and indentation
    EXPECT_NE(result.find('\n'), std::string::npos);
    EXPECT_NE(result.find("  "), std::string::npos);
    
    // Should be properly formatted
    EXPECT_NE(result.find("{\n"), std::string::npos);
    EXPECT_NE(result.find("\n}"), std::string::npos);
}

TEST_F(FormatTest, CustomIndentation) {
    format fmt(format_type::pretty);
    fmt.indent = 4;
    
    std::string result = test_obj.to_str(fmt);
    EXPECT_NE(result.find("    "), std::string::npos); // 4 spaces
}

TEST_F(FormatTest, ArrayFormatting) {
    std::string compact = test_arr.to_str();
    EXPECT_EQ(compact.find('\n'), std::string::npos);
    
    format fmt(format_type::pretty);
    std::string pretty = test_arr.to_str(fmt);
    EXPECT_NE(pretty.find('\n'), std::string::npos);
}

TEST_F(FormatTest, EmptyContainers) {
    value empty_obj;
    // Empty object is created by accessing with []
    empty_obj["dummy"];
    empty_obj.erase("dummy");
    EXPECT_EQ(empty_obj.to_str(), "{}");
    
    value empty_arr;
    // Empty array is created by calling append then clear
    empty_arr.append(1);
    empty_arr.erase(0);
    EXPECT_EQ(empty_arr.to_str(), "[]");
}

TEST_F(FormatTest, SpecialValues) {
    value null_val;
    EXPECT_THROW(null_val.as_str(), std::exception);
    
    value bool_val(true);
    EXPECT_EQ(bool_val.as_str(), "true");
    
    value false_val(false);
    EXPECT_EQ(false_val.as_str(), "false");
}
TEST_F(FormatTest, NestedStructures) {
    value nested;
    nested["user"] = test_obj;
    nested["items"] = test_arr;
    
    format fmt(format_type::pretty);
    std::string result = nested.to_str(fmt);
    
    // Should have proper nesting
    EXPECT_NE(result.find("{\n"), std::string::npos);
    EXPECT_NE(result.find("  \"user\" : {\n"), std::string::npos);
    EXPECT_NE(result.find("  \"items\" : [\n"), std::string::npos);
}

TEST_F(FormatTest, StringEscaping) {
    value obj;
    obj["msg"] = "Hello\nWorld\t\"Quote\"";
    std::string result = obj.to_str();
    
    EXPECT_NE(result.find("\\n"), std::string::npos);
    EXPECT_NE(result.find("\\t"), std::string::npos);
    EXPECT_NE(result.find("\\\""), std::string::npos);
}

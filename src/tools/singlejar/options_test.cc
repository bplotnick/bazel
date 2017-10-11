// Copyright 2016 The Bazel Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <memory>

#include "src/tools/singlejar/options.h"
#include "src/tools/singlejar/test_util.h"

#include "src/main/cpp/util/port.h"
#include "gtest/gtest.h"

TEST(OptionsTest, Flags1) {
  const char *args[] = {"--exclude_build_data",
                        "--compression",
                        "--normalize",
                        "--no_duplicates",
                        "--output", "output_jar"};
  Options options;
  options.ParseCommandLine(arraysize(args), args);

  EXPECT_TRUE(options.exclude_build_data);
  EXPECT_TRUE(options.force_compression);
  EXPECT_TRUE(options.normalize_timestamps);
  EXPECT_TRUE(options.no_duplicates);
  EXPECT_FALSE(options.preserve_compression);
  EXPECT_FALSE(options.verbose);
  EXPECT_FALSE(options.warn_duplicate_resources);
  EXPECT_FALSE(options.check_desugar_deps);
  EXPECT_EQ("output_jar", options.output_jar);
}

TEST(OptionsTest, Flags2) {
  const char *args[] = {"--dont_change_compression",
                        "--verbose",
                        "--warn_duplicate_resources",
                        "--check_desugar_deps",
                        "--output", "output_jar"};
  Options options;
  options.ParseCommandLine(arraysize(args), args);

  ASSERT_FALSE(options.exclude_build_data);
  ASSERT_FALSE(options.force_compression);
  ASSERT_FALSE(options.normalize_timestamps);
  ASSERT_FALSE(options.no_duplicates);
  ASSERT_TRUE(options.preserve_compression);
  ASSERT_TRUE(options.verbose);
  ASSERT_TRUE(options.warn_duplicate_resources);
  ASSERT_TRUE(options.check_desugar_deps);
}

TEST(OptionsTest, SingleOptargs) {
  const char *args[] = {"--output", "output_jar",
                        "--main_class", "com.google.Main",
                        "--java_launcher", "//tools:mylauncher",
                        "--build_info_file", "build_file1",
                        "--extra_build_info", "extra_build_line1",
                        "--build_info_file", "build_file2",
                        "--extra_build_info", "extra_build_line2"};
  Options options;
  options.ParseCommandLine(arraysize(args), args);

  EXPECT_EQ("output_jar", options.output_jar);
  EXPECT_EQ("com.google.Main", options.main_class);
  EXPECT_EQ("//tools:mylauncher", options.java_launcher);
  ASSERT_EQ(2, options.build_info_files.size());
  EXPECT_EQ("build_file1", options.build_info_files[0]);
  EXPECT_EQ("build_file2", options.build_info_files[1]);
  ASSERT_EQ(2, options.build_info_lines.size());
  EXPECT_EQ("extra_build_line1", options.build_info_lines[0]);
  EXPECT_EQ("extra_build_line2", options.build_info_lines[1]);
}

TEST(OptionsTest, MultiOptargs) {
    const char *args[] = {"--output", "output_file",
                        "--sources", "jar1", "jar2",
                        "--resources", "res1", "res2",
                        "--classpath_resources", "cpres1", "cpres2",
                        "--sources", "jar3",
                        "--include_prefixes", "prefix1", "prefix2",
                        "--nocompress_suffixes", ".png", ".so"};
  Options options;
  options.ParseCommandLine(arraysize(args), args);

  ASSERT_EQ(3, options.input_jars.size());
  EXPECT_EQ("jar1", options.input_jars[0]);
  EXPECT_EQ("jar2", options.input_jars[1]);
  EXPECT_EQ("jar3", options.input_jars[2]);
  ASSERT_EQ(2, options.resources.size());
  EXPECT_EQ("res1", options.resources[0]);
  EXPECT_EQ("res2", options.resources[1]);
  ASSERT_EQ(2, options.classpath_resources.size());
  EXPECT_EQ("cpres1", options.classpath_resources[0]);
  EXPECT_EQ("cpres2", options.classpath_resources[1]);
  ASSERT_EQ(2, options.include_prefixes.size());
  EXPECT_EQ("prefix1", options.include_prefixes[0]);
  EXPECT_EQ("prefix2", options.include_prefixes[1]);
  EXPECT_EQ(2, options.nocompress_suffixes.size());
  EXPECT_EQ(".png", options.nocompress_suffixes[0]);
  EXPECT_EQ(".so", options.nocompress_suffixes[1]);
}

TEST(OptionsTest, EmptyMultiOptargs) {
  const char *args[] = {"--output", "output_file",
                        "--sources",
                        "--resources",
                        "--classpath_resources",
                        "--sources",
                        "--include_prefixes", "prefix1",
                        "--resources"};
  Options options;
  options.ParseCommandLine(arraysize(args), args);

  EXPECT_EQ(0, options.input_jars.size());
  EXPECT_EQ(0, options.resources.size());
  EXPECT_EQ(0, options.classpath_resources.size());
  EXPECT_EQ(1, options.include_prefixes.size());
}

#include "pch.h"
#include "..\convert\phenotype_convert.h"
#include "..\convert\genotype_convert.h"
#include <fstream>
#include <sstream>
#include <algorithm>
TEST(Phenotype_parse, Initiate) {
    Discrete_phenotype_map discrete_phenotype_map;
    EXPECT_EQ(discrete_phenotype_map.status(), Phenotype_status::empty);
    Discrete_phenotype_map scalar_phenotype_map;
    EXPECT_EQ(scalar_phenotype_map.status(),Phenotype_status::empty);
}
TEST(Phenotype_parse, Discrete_line_parse) {
    Phenotype_flags sample_flags;

    sample_flags.delimiter = ',';
    sample_flags.is_discrete = true;
    sample_flags.UID_idx = 1;
    sample_flags.phenotype_idx = 28;
    std::string empty_phenotype_line = "1000092_90001_0_0_judging.csv,6.0,404.5,77.77103145344887,116.58333333333331,34.054388687640376,24.0,44.93328387732194,19.33333333333333,29.14856581186952,446.6428571428572,119.06442262361112,1.1666666666666667,0.3726779962499649,130.0,68.5,0.3333333333333333,0.4714045207910317,854.0,43.32724316177986,1439.9999999999961,0.5994024728740283,0.3033564814814815,6.0,63,0,0:0,,,66,11";
    std::string normal_phenotype_line = "1000183_90001_0_0_judging.csv,6.0,368.41666666666674,196.35138923765103,54.91666666666666,52.08439998907756,83.75,135.0116507318288,21.0,29.698484809834994,508.0,139.570054094709,0.8333333333333334,0.37267799624996506,209.5,9.228578799938086,0.5,0.7637626158259734,899.0833333333335,15.186113905655905,1439.9999999999961,0.6486698247354865,0.3168981481481481,6.0,56,0,1:0,3b,,61,9";
    auto epl_r = Phenotype_line_parser::parse_line(sample_flags,empty_phenotype_line);//epl_r: empty phenotype line_result
    auto npl_r = Phenotype_line_parser::parse_line(sample_flags,normal_phenotype_line);
    EXPECT_EQ(epl_r.first, "1000092_90001_0_0_judging.csv");
    EXPECT_EQ(npl_r.first, "1000183_90001_0_0_judging.csv");
    EXPECT_EQ(epl_r.second, "");
    EXPECT_EQ(npl_r.second, "3b");
}
TEST(Phenotype_parse, Scalar_line_parse) {
    Phenotype_flags sample_flags;

    sample_flags.delimiter = ',';
    sample_flags.is_discrete = false;
    sample_flags.UID_idx = 1;
    sample_flags.phenotype_idx = 4;
    std::string empty_phenotype_line = "1000092_90001_0_0_judging.csv,6.0,404.5,77.77103145344887,116.58333333333331,34.054388687640376,24.0,44.93328387732194,19.33333333333333,29.14856581186952,446.6428571428572,119.06442262361112,1.1666666666666667,0.3726779962499649,130.0,68.5,0.3333333333333333,0.4714045207910317,854.0,43.32724316177986,1439.9999999999961,0.5994024728740283,0.3033564814814815,6.0,63,0,0:0,,,66,11";
    std::string normal_phenotype_line = "1000183_90001_0_0_judging.csv,6.0,368.41666666666674,196.35138923765103,54.91666666666666,52.08439998907756,83.75,135.0116507318288,21.0,29.698484809834994,508.0,139.570054094709,0.8333333333333334,0.37267799624996506,209.5,9.228578799938086,0.5,0.7637626158259734,899.0833333333335,15.186113905655905,1439.9999999999961,0.6486698247354865,0.3168981481481481,6.0,56,0,1:0,3b,,61,9";
    auto npl_r = Phenotype_line_parser::parse_line(sample_flags,normal_phenotype_line);
    EXPECT_EQ(npl_r.first, "1000183_90001_0_0_judging.csv");
    EXPECT_EQ(npl_r.second, "196.35138923765103");
}
TEST(Phenotype_parse, Discrete_file_parse) {
    Phenotype_flags sample_flags;
    sample_flags.filename = "sample_phenotype.csv";
    sample_flags.delimiter = ',';
    sample_flags.is_discrete = true;
    sample_flags.UID_idx = 1;
    sample_flags.phenotype_idx = 28;
    sample_flags.skip_first_row = true;
    Discrete_phenotype_map discrete_phenotype_map;
    discrete_phenotype_map.read_phenotype(sample_flags);
    EXPECT_EQ(discrete_phenotype_map.size(), 100);
    EXPECT_EQ(discrete_phenotype_map.status(), Phenotype_status::bisect_wait);
    std::unordered_set<std::string> raw_phenotype_set = discrete_phenotype_map.get_raw_phenotype();
    std::vector<std::string> raw_phenotype_vec;
    raw_phenotype_vec.push_back("1");
    raw_phenotype_vec.push_back("5");
    raw_phenotype_vec.push_back("3a");
    raw_phenotype_vec.push_back("3b");
    raw_phenotype_vec.push_back("4a");
    raw_phenotype_vec.push_back("4b");
    EXPECT_EQ(raw_phenotype_set.size(), raw_phenotype_vec.size());
    bool not_found = false;
    for (const std::string& str : raw_phenotype_vec) {
        not_found = not_found || (raw_phenotype_set.find(str) == raw_phenotype_set.end());
    }
    EXPECT_FALSE(not_found);
}
TEST(Phenotype_parse, Discrete_phenotype_map_parse) {
    Phenotype_flags sample_flags;
    sample_flags.filename = "sample_phenotype.csv";
    sample_flags.delimiter = ',';
    sample_flags.is_discrete = true;
    sample_flags.UID_idx = 1;
    sample_flags.phenotype_idx = 28;
    sample_flags.skip_first_row = true;
    Discrete_phenotype_map discrete_phenotype_map;
    discrete_phenotype_map.read_phenotype(sample_flags);
    std::unordered_set<std::string> positive_phenotype_set;
    positive_phenotype_set.insert("1");
    positive_phenotype_set.insert("4b");
    auto phenotype_status =  discrete_phenotype_map.set_phenotype_map(positive_phenotype_set);
    EXPECT_EQ(phenotype_status, Phenotype_status::ready);
    EXPECT_EQ(1, discrete_phenotype_map.find_discrete_state("1005905_90001_0_0_judging.csv").second);//4b
    EXPECT_EQ(1, discrete_phenotype_map.find_discrete_state("1000500_90001_0_0_judging.csv").second);//1
    EXPECT_FALSE(discrete_phenotype_map.find_discrete_state("1000500_90001_0_1_judging.csv").first);//Non-existent
    EXPECT_EQ(0, discrete_phenotype_map.find_discrete_state("1000554_90001_0_0_judging.csv").second);//3b
    std::ostringstream oss;
    discrete_phenotype_map.print(oss);
    std::string phenotype_print_lines_str = oss.str();
    std::vector<std::string> phenotype_print_lines_vec;
    EXPECT_EQ(phenotype_print_lines_str.size(), 62 * 100);
    for (int i = 0; i < 100; i++) {
        std::string part_line = phenotype_print_lines_str.substr(62 * i, 62);
        phenotype_print_lines_vec.push_back(part_line);
    }
    std::sort(phenotype_print_lines_vec.begin(), phenotype_print_lines_vec.end());
    EXPECT_EQ(phenotype_print_lines_vec[0], "1000092_90001_0_0_judging.csv 1000092_90001_0_0_judging.csv 1\n");
    EXPECT_EQ(phenotype_print_lines_vec[1], "1000183_90001_0_0_judging.csv 1000183_90001_0_0_judging.csv 0\n");
}
TEST(Phenotype_parse, Scalar_file_parse) {
    Phenotype_flags sample_flags;
    sample_flags.filename = "sample_phenotype.csv";
    sample_flags.delimiter = ',';
    sample_flags.is_discrete = true;
    sample_flags.UID_idx = 1;
    sample_flags.phenotype_idx = 4;
    sample_flags.skip_first_row = true;
    Scalar_phenotype_map scalar_phenotype_map;
    scalar_phenotype_map.read_phenotype(sample_flags);
    EXPECT_EQ(scalar_phenotype_map.size(), 100);
    EXPECT_EQ(scalar_phenotype_map.status(), Phenotype_status::ready);
    EXPECT_NEAR(34.54154, scalar_phenotype_map.find_scalar_state("1005905_90001_0_0_judging.csv").second, 1e-5);
}
TEST(Genotype_subject_parse, Genotype_line_parse) {
    Genotype_subject_flags gs_flag;
    std::ifstream fin("Genotype_line_parse_input.txt");
    std::string genotype_line;
    std::getline(fin, genotype_line);
    std::string uid_str;
    fin >> uid_str;
    std::string genotype_str;
    fin >> genotype_str;
    gs_flag.delimiter = '\t';
    gs_flag.UID_idx = 1;
    gs_flag.genotype_idx = 2;
    const auto parse_result = Genotype_subject_line_parser::parse_line(gs_flag,genotype_line);
    EXPECT_EQ(uid_str, parse_result.first);
    EXPECT_EQ(genotype_str,parse_result.second);
}
TEST(Genotype_map_parse, Genotype_map_line_parse) {
    Genotype_map_flags gm_flag;
    std::string gm_line;
    
    gm_flag.delimiter = '\t';
    gm_flag.SNP_idx = 1;
    gm_flag.base_start_idx = 4;
    gm_line = "80278591\t10\t0\t\"GAC G\"\t\"0 0\"\t\"G GAC\"\t\"\"";
    auto parse_result = Genotype_line_parser::parse_line_for_original(gm_flag,gm_line);
    auto proxy_parse_result = Genotype_line_parser::parse_line_for_proxy(gm_flag,gm_line);
    EXPECT_EQ("80278591", parse_result.first);
    EXPECT_EQ("GAC G", parse_result.second[0]);
    EXPECT_EQ("0 0", parse_result.second[1]);
    EXPECT_EQ("G GAC", parse_result.second[2]);
    EXPECT_EQ("", parse_result.second[3]);
    EXPECT_EQ("1 2", proxy_parse_result.second[0]);
    EXPECT_EQ("0 0", proxy_parse_result.second[1]);
    EXPECT_EQ("2 1", proxy_parse_result.second[2]);
    EXPECT_EQ("E E", proxy_parse_result.second[3]);
    gm_line = "2795269\t10\t15\t\"T C\"\t\"C C\"\t\"T T\"\t\"0 0\"";
    parse_result = Genotype_line_parser::parse_line_for_original(gm_flag, gm_line);
    proxy_parse_result = Genotype_line_parser::parse_line_for_proxy(gm_flag, gm_line);
    EXPECT_EQ("2795269", parse_result.first);
    EXPECT_EQ("T C", parse_result.second[0]);
    EXPECT_EQ("C C", parse_result.second[1]);
    EXPECT_EQ("T T", parse_result.second[2]);
    EXPECT_EQ("0 0", parse_result.second[3]);
    EXPECT_EQ("1 2", proxy_parse_result.second[0]);
    EXPECT_EQ("2 2", proxy_parse_result.second[1]);
    EXPECT_EQ("1 1", proxy_parse_result.second[2]);
    EXPECT_EQ("0 0", proxy_parse_result.second[3]);
    gm_line = "35481888\t10\t12\tG G\tA G\tA A\t0 0";//Quotation Free version
    parse_result = Genotype_line_parser::parse_line_for_original(gm_flag, gm_line);
    proxy_parse_result = Genotype_line_parser::parse_line_for_proxy(gm_flag, gm_line);
    EXPECT_EQ("35481888", parse_result.first);
    EXPECT_EQ("G G", parse_result.second[0]);
    EXPECT_EQ("A G", parse_result.second[1]);
    EXPECT_EQ("A A", parse_result.second[2]);
    EXPECT_EQ("0 0", parse_result.second[3]);
    EXPECT_EQ("1 1", proxy_parse_result.second[0]);
    EXPECT_EQ("2 1", proxy_parse_result.second[1]);
    EXPECT_EQ("2 2", proxy_parse_result.second[2]);
    EXPECT_EQ("0 0", proxy_parse_result.second[3]);
    gm_line = "80278592\t10\t4\t\"AG\"\t\"0 0\"\t\"A A\"\t\"\"";//No space base version
    parse_result = Genotype_line_parser::parse_line_for_original(gm_flag, gm_line);
    proxy_parse_result = Genotype_line_parser::parse_line_for_proxy(gm_flag, gm_line);
    EXPECT_EQ("80278592", parse_result.first);
    EXPECT_EQ("AG", parse_result.second[0]);
    EXPECT_EQ("0 0", parse_result.second[1]);
    EXPECT_EQ("A A", parse_result.second[2]);
    EXPECT_EQ("", parse_result.second[3]);
    EXPECT_EQ("1 2", proxy_parse_result.second[0]);
    EXPECT_EQ("0 0", proxy_parse_result.second[1]);
    EXPECT_EQ("1 1", proxy_parse_result.second[2]);
    EXPECT_EQ("E E", proxy_parse_result.second[3]);
}
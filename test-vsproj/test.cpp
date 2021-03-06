#include "pch.h"
#include "..\convert\phenotype_convert.h"
#include "..\convert\genotype_convert.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <mutex>
#include <thread>
#include <numeric>
#include <execution>
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
    const std::string filename = "sample_phenotype.csv";
    sample_flags.delimiter = ',';
    sample_flags.is_discrete = true;
    sample_flags.UID_idx = 1;
    sample_flags.phenotype_idx = 28;
    sample_flags.skip_first_row = true;
    Discrete_phenotype_map discrete_phenotype_map;
    discrete_phenotype_map.read_phenotype(filename,sample_flags);
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
    const std::string filename = "sample_phenotype.csv";
    sample_flags.delimiter = ',';
    sample_flags.is_discrete = true;
    sample_flags.UID_idx = 1;
    sample_flags.phenotype_idx = 28;
    sample_flags.skip_first_row = true;
    Discrete_phenotype_map discrete_phenotype_map;
    discrete_phenotype_map.read_phenotype(filename,sample_flags);
    std::unordered_set<std::string> positive_phenotype_set;
    positive_phenotype_set.insert("1");
    positive_phenotype_set.insert("4b");
    auto phenotype_status =  discrete_phenotype_map.set_phenotype_map(positive_phenotype_set);
    EXPECT_EQ(phenotype_status, Phenotype_status::ready);
    EXPECT_EQ(2, discrete_phenotype_map.find_discrete_state("1005905_90001_0_0_judging.csv").second);//4b
    EXPECT_EQ(2, discrete_phenotype_map.find_discrete_state("1000500_90001_0_0_judging.csv").second);//1
    EXPECT_FALSE(discrete_phenotype_map.find_discrete_state("1000500_90001_0_1_judging.csv").first);//Non-existent
    EXPECT_EQ(1, discrete_phenotype_map.find_discrete_state("1000554_90001_0_0_judging.csv").second);//3b
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
    EXPECT_EQ(phenotype_print_lines_vec[0], "1000092_90001_0_0_judging.csv 1000092_90001_0_0_judging.csv 2\n");
    EXPECT_EQ(phenotype_print_lines_vec[1], "1000183_90001_0_0_judging.csv 1000183_90001_0_0_judging.csv 1\n");
}
TEST(Phenotype_parse, Scalar_file_parse) {
    Phenotype_flags sample_flags;
    const std::string filename = "sample_phenotype.csv";
    sample_flags.delimiter = ',';
    sample_flags.is_discrete = true;
    sample_flags.UID_idx = 1;
    sample_flags.phenotype_idx = 4;
    sample_flags.skip_first_row = true;
    Scalar_phenotype_map scalar_phenotype_map;
    scalar_phenotype_map.read_phenotype(filename, sample_flags);
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
TEST(Genotype_proxy_parse, Genotype_proxy_line_parse) {
    Genotype_proxy_flags gp_flag;
    std::string gp_line;
    
    gp_flag.delimiter = '\t';
    gp_flag.SNP_idx = 1;
    gp_flag.chromosome_idx = 2;
    gp_flag.base_start_idx = 4;
    gp_line = "80278591\t10\t0\t\"GAC G\"\t\"0 0\"\t\"G GAC\"\t\"\"\n";
    auto parse_result = Genotype_line_parser::parse_line_for_original(gp_flag,gp_line);
    auto proxy_parse_result = Genotype_line_parser::parse_line_for_proxy(gp_flag,gp_line);
    auto chromosome_result = Genotype_line_parser::parse_chromosome(gp_flag, gp_line);
    EXPECT_EQ("80278591", parse_result.first);
    EXPECT_EQ("GAC G", parse_result.second[0]);
    EXPECT_EQ("0 0", parse_result.second[1]);
    EXPECT_EQ("G GAC", parse_result.second[2]);
    EXPECT_EQ(3, parse_result.second.size());
    EXPECT_EQ("1 2", proxy_parse_result.second[0]);
    EXPECT_EQ("0 0", proxy_parse_result.second[1]);
    EXPECT_EQ("2 1", proxy_parse_result.second[2]);
    EXPECT_EQ(3, proxy_parse_result.second.size());
    EXPECT_EQ("10", chromosome_result);
    gp_line = "2795269\t10\t15\t\"T C\"\t\"C C\"\t\"T T\"\t\"0 0\"";
    parse_result = Genotype_line_parser::parse_line_for_original(gp_flag, gp_line);
    proxy_parse_result = Genotype_line_parser::parse_line_for_proxy(gp_flag, gp_line);
    EXPECT_EQ("2795269", parse_result.first);
    EXPECT_EQ("T C", parse_result.second[0]);
    EXPECT_EQ("C C", parse_result.second[1]);
    EXPECT_EQ("T T", parse_result.second[2]);
    EXPECT_EQ("0 0", parse_result.second[3]);
    EXPECT_EQ("1 2", proxy_parse_result.second[0]);
    EXPECT_EQ("2 2", proxy_parse_result.second[1]);
    EXPECT_EQ("1 1", proxy_parse_result.second[2]);
    EXPECT_EQ("0 0", proxy_parse_result.second[3]);
    gp_line = "35481888\t10\t12\tG G\tA G\tA A\t0 0";//Quotation Free version
    parse_result = Genotype_line_parser::parse_line_for_original(gp_flag, gp_line);
    proxy_parse_result = Genotype_line_parser::parse_line_for_proxy(gp_flag, gp_line);
    EXPECT_EQ("35481888", parse_result.first);
    EXPECT_EQ("G G", parse_result.second[0]);
    EXPECT_EQ("A G", parse_result.second[1]);
    EXPECT_EQ("A A", parse_result.second[2]);
    EXPECT_EQ("0 0", parse_result.second[3]);
    EXPECT_EQ("1 1", proxy_parse_result.second[0]);
    EXPECT_EQ("2 1", proxy_parse_result.second[1]);
    EXPECT_EQ("2 2", proxy_parse_result.second[2]);
    EXPECT_EQ("0 0", proxy_parse_result.second[3]);
    gp_line = "80278592\t10\t4\t\"AG\"\t\"0 0\"\t\"A A\"\t\"\"";//No space base version
    parse_result = Genotype_line_parser::parse_line_for_original(gp_flag, gp_line);
    proxy_parse_result = Genotype_line_parser::parse_line_for_proxy(gp_flag, gp_line);
    EXPECT_EQ("80278592", parse_result.first);
    EXPECT_EQ("AG", parse_result.second[0]);
    EXPECT_EQ("0 0", parse_result.second[1]);
    EXPECT_EQ("A A", parse_result.second[2]);
    EXPECT_EQ(3, parse_result.second.size());
    EXPECT_EQ("1 2", proxy_parse_result.second[0]);
    EXPECT_EQ("0 0", proxy_parse_result.second[1]);
    EXPECT_EQ("1 1", proxy_parse_result.second[2]);
    EXPECT_EQ(3, proxy_parse_result.second.size()); 
}
TEST(Genotype_proxy_parse, Genotype_proxy_file_parse) {
    Genotype_proxy_flags gp_flag;

    const std::string map_filename = "dic_head.dat";
    gp_flag.skip_first_row = false;
    gp_flag.delimiter = '\t';
    gp_flag.SNP_idx = 1;
    gp_flag.chromosome_idx = 2;
    gp_flag.base_start_idx = 4;
    Genotype_proxy_map genotype_proxy_map;
    EXPECT_TRUE(genotype_proxy_map.read_map(map_filename,gp_flag));
    EXPECT_EQ(genotype_proxy_map.size(), 500);
    //Check some proxy base pairs: 
    EXPECT_EQ(genotype_proxy_map.get_SNP_name(499).second, "2836653");
    EXPECT_EQ(genotype_proxy_map.get_SNP_name(500).first, false);
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(499, 0).second, "1 1");//T T
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(499, 1).second, "2 1");//C T
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(499, 2).second, "2 2");//C C
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(499, 3).second, "0 0");//0 0
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(497, 0).second, "1 1");//A A
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(497, 1).second, "0 0");//0 0
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(497, 2).second, "2 1");//G A
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(497, 3).first, false);//_, CANNOT be read as this index should not exist.
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(376, 0).second, "1 1");//C C
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(376, 1).second, "2 1");//CAGT C
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(376, 2).second, "0 0");//0 0
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(376, 3).second, "2 3");//CAGT CA
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(317, 0).second, "0 0");//0 0
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(317, 1).second, "1 2");//GA
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(317, 2).second, "1 1");//G G
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele(317, 3).first, false);//_, CANNOT be read as this index should not exist.
}
TEST(Genotype_proxy_parse, Genotype_proxy_large_file_parse) {
    Genotype_proxy_flags gp_flag;

    const std::string map_filename = "gene_dic.dat";
    gp_flag.skip_first_row = false;
    gp_flag.delimiter = '\t';
    gp_flag.SNP_idx = 1;
    gp_flag.chromosome_idx = 2;
    gp_flag.base_start_idx = 4;
    Genotype_proxy_map genotype_proxy_map;
    EXPECT_TRUE(genotype_proxy_map.read_map(map_filename, gp_flag));
}
TEST(Genotype_proxy_parse, Genotype_proxy_file_print) {
    Genotype_proxy_flags gp_flag;

    const std::string map_filename = "dic_head.dat";
    gp_flag.skip_first_row = false;
    gp_flag.delimiter = '\t';
    gp_flag.SNP_idx = 1;
    gp_flag.chromosome_idx = 2;
    gp_flag.base_start_idx = 4;
    Genotype_proxy_map genotype_proxy_map;
    EXPECT_TRUE(genotype_proxy_map.read_map(map_filename, gp_flag));
    EXPECT_EQ(genotype_proxy_map.size(), 500);
    std::string proxy_map_str;
    std::ostringstream oss;
    genotype_proxy_map.print(oss);
    proxy_map_str = oss.str();
    std::istringstream iss(proxy_map_str);
    std::string line;
    std::getline(iss, line, '\n');
    EXPECT_EQ(line, "10 80278591 0 1");
    std::getline(iss, line, '\n');
    EXPECT_EQ(line, "10 52134431 0 2");
}
TEST(Genotype_proxy_parse, Genotype_proxy_raw_genotype_convert) {
    Genotype_proxy_flags gp_flag;

    const std::string map_filename = "short_dic_head.dat";
    //has 13 SNPs, (1-based) position 8,9,12,13 have only 3 alleles.
    gp_flag.skip_first_row = false;
    gp_flag.delimiter = '\t';
    gp_flag.SNP_idx = 1;
    gp_flag.chromosome_idx = 2;
    gp_flag.base_start_idx = 4;
    Genotype_proxy_map genotype_proxy_map;
    EXPECT_TRUE(genotype_proxy_map.read_map(map_filename, gp_flag));
    EXPECT_EQ(genotype_proxy_map.size(), 13);
    EXPECT_EQ(genotype_proxy_map.get_allele_count(7).second, 3);
    EXPECT_EQ(genotype_proxy_map.get_allele_count(8).second, 3);
    EXPECT_EQ(genotype_proxy_map.get_allele_count(11).second, 3);
    EXPECT_EQ(genotype_proxy_map.get_allele_count(12).second, 3);
    std::string short_raw = "012012012012";//length 12
    std::string long_raw = "01201201201201";//length 14
    std::string ob_raw = "0120120120123";//index 13 over bound
    EXPECT_FALSE(genotype_proxy_map.get_proxy_allele_line(short_raw).first);
    EXPECT_FALSE(genotype_proxy_map.get_proxy_allele_line(long_raw).first);
    EXPECT_FALSE(genotype_proxy_map.get_proxy_allele_line(ob_raw).first);
    std::string valid_raw = "0120120120120";
    std::string valid_proxy = "1 1 2 1 1 1 1 1 2 1 1 1 1 1 0 0 0 0 1 2 2 2 2 1 1 1";
    EXPECT_EQ(genotype_proxy_map.get_proxy_allele_line(valid_raw).second, valid_proxy);
}
TEST(Genotype_file_converter, Genotype_stream_converter_test) {
    //Set phenotype map: 
    Phenotype_flags p_flag;
    const std::string filename = "sample_phenotype.csv";
    p_flag.delimiter = ',';
    p_flag.is_discrete = true;
    p_flag.UID_idx = 1;
    p_flag.phenotype_idx = 28;
    p_flag.skip_first_row = true;
    Discrete_phenotype_map discrete_phenotype_map;
    discrete_phenotype_map.read_phenotype(filename, p_flag);
    std::unordered_set<std::string> positive_phenotype_set;
    positive_phenotype_set.insert("1");
    positive_phenotype_set.insert("4b");
    auto phenotype_status = discrete_phenotype_map.set_phenotype_map(positive_phenotype_set);
    EXPECT_EQ(phenotype_status, Phenotype_status::ready);
    //Set genotype proxy map:
    Genotype_proxy_flags gp_flag;
    const std::string map_filename = "short_dic_head.dat";
    //has 13 SNPs, (1-based) position 8,9,12,13 have only 3 alleles.
    gp_flag.skip_first_row = false;
    gp_flag.delimiter = '\t';
    gp_flag.SNP_idx = 1;
    gp_flag.chromosome_idx = 2;
    gp_flag.base_start_idx = 4;
    Genotype_proxy_map genotype_proxy_map;
    EXPECT_TRUE(genotype_proxy_map.read_map(map_filename, gp_flag));
    EXPECT_EQ(genotype_proxy_map.size(), 13);
    //Set genotype subject flags. 
    Genotype_subject_flags gs_flag;
    gs_flag.skip_first_row = false;
    gs_flag.delimiter = '\t';
    gs_flag.UID_idx = 1;
    gs_flag.genotype_idx = 2;
    std::string genotype_line = "1000500_90001_0_0_judging.csv\t0120120120120";
    
    std::istringstream is(genotype_line);
    std::ostringstream os;
    //Check case without phenotype: 
    Genotype_file_converter sp_gfc(&genotype_proxy_map);//sans phenotype genotype file converter:
    std::string ref_parse_result_no_phenotype = std::string("1000500_90001_0_0_judging.csv ") + "1 1 2 1 1 1 1 1 2 1 1 1 1 1 0 0 0 0 1 2 2 2 2 1 1 1\n";
    EXPECT_EQ(sp_gfc.is_valid(), Genotype_file_convert_status::success);
    EXPECT_EQ(sp_gfc.convert(is, os, gs_flag), Genotype_file_convert_status::success);
    EXPECT_EQ(os.str(), ref_parse_result_no_phenotype);
}
namespace biased_dist {
    std::discrete_distribution<> d0({ 1, 1 });
    std::discrete_distribution<> d1({ 3, 1 });
    std::discrete_distribution<> d2({ 1, 3 });
    std::discrete_distribution<> d3({ 3, 2 });
}
int biased_phenotype(const int allele_type_idx, std::mt19937& gen) {
    //Returns 0 and 1, randomly, distribution is dependent on allele_type_idx. 
    using namespace biased_dist;
    switch (allele_type_idx%4)
    {
    case 0:
        return d0(gen);
    case 1:
        return d1(gen);
    case 2:
        return d2(gen);
    case 3:
        return d3(gen);
    }
}
std::pair <std::string, std::string> generate_genotype_phenotype_subject_line(const int uid, const int bias_index, const Genotype_proxy_map& genotype_proxy_map) {
    std::string pt_line;
    std::string gt_line;
    std::random_device rd;
    std::mt19937 gen(rd());
    gt_line.append(std::to_string(uid)+"\t");
    pt_line.append(std::to_string(uid) + ",");
    const int M = genotype_proxy_map.size();
    for (int j = 0; j < M; j++) {
        const int allele_count = genotype_proxy_map.get_allele_count(j).second;
        //Recall: allele_type_idx is 0-based, convention used by UK Biobank
        std::uniform_int_distribution<> ui_dist(0, allele_count - 1);
        const int rnd_val = ui_dist(gen);
        gt_line.append(std::to_string(rnd_val));
        if (j == bias_index) {
            pt_line.append(std::to_string(biased_phenotype(rnd_val, gen)));
        }
    }
    pt_line.append(1, '\n');
    gt_line.append(1, '\n');
    return make_pair(pt_line, gt_line);
}
bool generate_genotype_phenotype_subject(const Genotype_proxy_map& genotype_proxy_map, const int subject_count,const std::string& out_genotype_filename,
    const int bias_index, const std::string& out_phenotype_filename) {

    std::ofstream gt_fs;
    std::ofstream pt_fs;
    gt_fs.open(out_genotype_filename, std::ios::trunc);
    pt_fs.open(out_phenotype_filename, std::ios::trunc);
    if (!gt_fs.is_open() || !pt_fs.is_open()) {
        return false;
    }
    
    //In the generated file, each subject has UID [ 1, subject_count], with raw genotype of length M separated by \t
    std::vector<int> uid_vector(subject_count);
    std::iota(uid_vector.begin(), uid_vector.end(), 1);
    std::mutex fs_mutex;
    std::for_each(std::execution::par_unseq,uid_vector.begin(), uid_vector.end(), [&](const int uid) {
        const auto line_pair = generate_genotype_phenotype_subject_line(uid + 1, bias_index, genotype_proxy_map);
        std::lock_guard<std::mutex> lock(fs_mutex);
        pt_fs << line_pair.first;
        gt_fs << line_pair.second;
        });
    pt_fs.close();
    gt_fs.close();
    return true;
}
TEST(Genotype_file_converter, Genotype_file_converter_test) {
    //Set genotype proxy map:
    Genotype_proxy_flags gp_flag;
    const std::string map_filename = "dic_head.dat";
    //has 13 SNPs, (1-based) position 8,9,12,13 have only 3 alleles.
    gp_flag.skip_first_row = false;
    gp_flag.delimiter = '\t';
    gp_flag.SNP_idx = 1;
    gp_flag.chromosome_idx = 2;
    gp_flag.base_start_idx = 4;
    Genotype_proxy_map genotype_proxy_map;
    EXPECT_TRUE(genotype_proxy_map.read_map(map_filename, gp_flag));
    EXPECT_EQ(genotype_proxy_map.size(), 500);
    //Set genotype subject flags. 
    Genotype_subject_flags gs_flag;
    gs_flag.skip_first_row = false;
    gs_flag.delimiter = '\t';
    gs_flag.UID_idx = 1;
    gs_flag.genotype_idx = 2;
    const std::string g_filename = "rnd_genotype.csv";
    //Generate genotype subject and phenotype map from data: 
    const std::string p_filename = "rnd_phenotype.csv";
    EXPECT_TRUE(generate_genotype_phenotype_subject(genotype_proxy_map, 2000, g_filename, 18, p_filename));
    //Set phenotype map: 
    Phenotype_flags p_flag;
    p_flag.delimiter = ',';
    p_flag.is_discrete = true;
    p_flag.UID_idx = 1;
    p_flag.phenotype_idx = 2;
    p_flag.skip_first_row = false;
    Discrete_phenotype_map discrete_phenotype_map;
    discrete_phenotype_map.read_phenotype(p_filename, p_flag);
    std::unordered_set<std::string> positive_phenotype_set;
    positive_phenotype_set.insert("1");
    auto phenotype_status = discrete_phenotype_map.set_phenotype_map(positive_phenotype_set);
    EXPECT_EQ(phenotype_status, Phenotype_status::ready);
    //Output proxy map: 
    const std::string pxm_filename = "proxy.map";
    EXPECT_TRUE(genotype_proxy_map.print(pxm_filename));
    std::ifstream gt_fs;
    gt_fs.open(g_filename,std::ios_base::in);
    EXPECT_TRUE(gt_fs.is_open());
    std::ofstream px_fs;//proxy filestream
    const std::string px_filename = "proxy.ped";
    px_fs.open(px_filename, std::ios::trunc);
    EXPECT_TRUE(px_fs.is_open());
    //Check case with phenotype: (Currently, output to files, then run in plink)
    Genotype_file_converter p_gfc(&genotype_proxy_map, static_cast<Phenotype_map*>(&discrete_phenotype_map));
    EXPECT_EQ(p_gfc.is_valid(), Genotype_file_convert_status::success);
    EXPECT_EQ(p_gfc.convert(gt_fs, px_fs, gs_flag), Genotype_file_convert_status::success);
}
TEST(Genotype_file_converter, Genotype_large_file_converter_test) {
    //Set genotype proxy map:
    Genotype_proxy_flags gp_flag;
    const std::string map_filename = "gene_dic.dat";
    gp_flag.skip_first_row = false;
    gp_flag.delimiter = '\t';
    gp_flag.SNP_idx = 1;
    gp_flag.chromosome_idx = 2;
    gp_flag.base_start_idx = 4;
    Genotype_proxy_map genotype_proxy_map;
    EXPECT_TRUE(genotype_proxy_map.read_map(map_filename, gp_flag));
    EXPECT_EQ(genotype_proxy_map.size(), 805426);
    //Set genotype subject flags. 
    Genotype_subject_flags gs_flag;
    gs_flag.skip_first_row = false;
    gs_flag.delimiter = '\t';
    gs_flag.UID_idx = 1;
    gs_flag.genotype_idx = 2;
    const std::string g_filename = "rnd_genotype.csv";
    //Generate genotype subject and phenotype map from data: 
    const std::string p_filename = "rnd_phenotype.csv";
    EXPECT_TRUE(generate_genotype_phenotype_subject(genotype_proxy_map, 20000, g_filename, 18, p_filename));
    //Set phenotype map: 
    Phenotype_flags p_flag;
    p_flag.delimiter = ',';
    p_flag.is_discrete = true;
    p_flag.UID_idx = 1;
    p_flag.phenotype_idx = 2;
    p_flag.skip_first_row = false;
    Discrete_phenotype_map discrete_phenotype_map;
    discrete_phenotype_map.read_phenotype(p_filename, p_flag);
    std::unordered_set<std::string> positive_phenotype_set;
    positive_phenotype_set.insert("1");
    auto phenotype_status = discrete_phenotype_map.set_phenotype_map(positive_phenotype_set);
    EXPECT_EQ(phenotype_status, Phenotype_status::ready);
    //Output proxy map: 
    const std::string pxm_filename = "proxy_large.map";
    EXPECT_TRUE(genotype_proxy_map.print(pxm_filename));
    std::ifstream gt_fs;
    gt_fs.open(g_filename, std::ios_base::in);
    EXPECT_TRUE(gt_fs.is_open());
    std::ofstream px_fs;//proxy filestream
    const std::string px_filename = "proxy_large.ped";
    px_fs.open(px_filename, std::ios::trunc);
    EXPECT_TRUE(px_fs.is_open());
    //Check case with phenotype: 
    Genotype_file_converter p_gfc(&genotype_proxy_map,static_cast<Phenotype_map*>(&discrete_phenotype_map));//phenotype genotype file converter:
    //std::string ref_parse_result_no_phenotype = std::string("1000500_90001_0_0_judging.csv ") + "1 1 2 1 1 1 1 1 2 1 1 1 1 1 0 0 0 0 1 2 2 2 2 1 1 1\n";
    EXPECT_EQ(p_gfc.is_valid(), Genotype_file_convert_status::success);
    EXPECT_EQ(p_gfc.convert(gt_fs, px_fs, gs_flag), Genotype_file_convert_status::success);
}
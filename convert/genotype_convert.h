#pragma once
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <utility>
#include "phenotype_convert.h"
enum class Genotype_status{empty,table_wait,ready,spoiled};
struct Genotype_subject_flags : private Line_parser{
    std::string filename;
    int UID_idx;
    int genotype_idx;
    char delimiter;
    bool skip_first_row;
    //Highlights genotype_field: (Reserved for GUI features) Returns start_idx,count pair
    std::pair<int, int> find_genotype_idx_range(const std::string& line)const;
    //Highlights UID field: (Reserved for GUI features) Returns start_idx,count pair
    std::pair<int, int> find_UID_idx_range(const std::string& line)const;
    //Returns pair of UID and genotype:
    std::pair<std::string, std::string> parse_line(const std::string& line) const;
};
struct Genotype_map_flags : private Line_parser{
    //Expected genotype map line: 
    //... uid_field ... 4 consecutive base_pair_field(e.g. "CCTGG CCT""C CCTGG" "0 0" "") ...
    //Expect genotype_map is ordered the same as the genotype sequence, no attempt is made to match index. 
    std::string map_filename;
    int SNP_idx;
    int base_start_idx;
    char delimiter;
    bool skip_first_row;
    //Highlights SNP field (Reserved for GUI features) Returns start_idx, count pair
    std::pair<int, int> find_SNP_idx_range(const std::string& line)const;
    //Highlights all (base_choice_idx)th Base pair field (Reserved for GUI features) Returns start_idx, count pair
    // base_choice_idx = 1,2,3,4
    std::pair<int, int> find_base_idx_range(const std::string& line,const int base_choice_idx)const;//
    //Returns pair of SNP idx, and vector of 4 states in original form.
    std::pair<std::string, std::vector<std::string>> parse_line_for_original(const std::string& line) const;
    //Returns pair of SNP idx, and vector of 4 states in its proxy form.
    //Map is distinct for each line, ordered by their apperance in the line.
    //Except that:
    //1. "0" is always mapped to 0 independent of its apperance order. 
    //2. "" should not appear in the result for later use, as "" only appears when there are less than 4 base choices.
    // We map it to "E" to indicate that there may be error. This can be used for detecting misalignment of the maps. 
    std::pair<std::string, std::vector<std::string>> parse_line_for_proxy(const std::string& line) const;
};
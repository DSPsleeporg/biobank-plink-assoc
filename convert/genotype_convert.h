//This header file defines the genotype mapping methods.
//The genotype mapping 

#pragma once
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <utility>
#include "phenotype_convert.h"
enum class Genotype_proxy_status{empty,ready,spoiled};
struct Genotype_subject_flags{
    std::string filename;
    int UID_idx;
    int genotype_idx;
    char delimiter;
    bool skip_first_row;
};
class Genotype_subject_line_parser : private Line_parser{
public:
    //Highlights genotype_field: (Reserved for GUI features) Returns start_idx,count pair
    static std::pair<int, int> find_genotype_idx_range(const Genotype_subject_flags& genotype_subject_flags, const std::string& line);
    //Highlights UID field: (Reserved for GUI features) Returns start_idx,count pair
    static std::pair<int, int> find_UID_idx_range(const Genotype_subject_flags& genotype_subject_flags, const std::string& line);
    //Returns pair of UID and genotype:
    static std::pair<std::string, std::string> parse_line(const Genotype_subject_flags& genotype_subject_flags, const std::string& line);
};
struct Genotype_proxy_flags{
    //Expected genotype map line: 
    //... uid_field ... 4 consecutive base_pair_field(e.g. "CCTGG CCT""C CCTGG" "0 0" "") ...
    //Expect genotype_map is ordered the same as the genotype sequence, no attempt is made to match index. 
    std::string map_filename;
    int SNP_idx;
    int base_start_idx;
    char delimiter;
    bool skip_first_row;
};
class Genotype_line_parser : private Line_parser {
public:
    //Highlights SNP field (Reserved for GUI features) Returns start_idx, count pair
    static std::pair<int, int> find_SNP_idx_range(const Genotype_proxy_flags& genotype_map_flags, const std::string& line);
    //Highlights all (base_choice_idx)th Base pair field (Reserved for GUI features) Returns start_idx, count pair
    // base_choice_idx = 1,2,3,4
    static std::pair<int, int> find_base_idx_range(const Genotype_proxy_flags& genotype_map_flags, const std::string& line,const int base_choice_idx);//
    //Returns pair of SNP idx, and vector of 4 states in original form.
    static std::pair<std::string, std::vector<std::string>> parse_line_for_original(const Genotype_proxy_flags& genotype_map_flags, const std::string& line);
    //Returns pair of SNP idx, and vector of 4 states in its proxy form.
    //Map is distinct for each line, ordered by their apperance in the line.
    //Except that:
    //1. "0" is always mapped to 0 independent of its apperance order. 
    //2. "" should not appear in the result for later use, as "" only appears when there are less than 4 base choices.
    // We map it to "E" to indicate that there may be error. This can be used for detecting misalignment of the maps. 
    static std::pair<std::string, std::vector<std::string>> parse_line_for_proxy(const Genotype_proxy_flags& genotype_map_flags, const std::string& line);
};
class Genotype_proxy_map {
    Genotype_proxy_status _genotype_proxy_status;
public: 
    //Constructor: creates empty map
    Genotype_proxy_map() :_genotype_proxy_status(Genotype_proxy_status::empty) {}
    //Number of subjects: 
};
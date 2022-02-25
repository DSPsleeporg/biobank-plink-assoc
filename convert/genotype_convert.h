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
    int SNP_idx;
    int base_start_idx;
    char delimiter;
    bool skip_first_row;
};
class Genotype_line_parser : private Line_parser {
public:
    //Highlights SNP field (Reserved for GUI features) Returns start_idx, count pair
    static std::pair<int, int> find_SNP_idx_range(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line);
    //Highlights all (base_choice_idx)th Base pair field (Reserved for GUI features) Returns start_idx, count pair
    // base_choice_idx = 1,2,3,4
    static std::pair<int, int> find_base_idx_range(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line,const int base_choice_idx);//
    //Returns pair of SNP idx, and vector of 4 states in original form.
    static std::pair<std::string, std::vector<std::string>> parse_line_for_original(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line);
    //Returns pair of SNP idx, and vector of 4 states in its proxy form.
    //Map is distinct for each line, ordered by their apperance in the line.
    //Except that:
    //1. "0" is always mapped to 0 independent of its apperance order. 
    //2. "" should not appear in the result for later use, as "" only appears when there are less than 4 base choices.
    // We map it to "E" to indicate that there may be error. This can be used for detecting misalignment of the maps. 
    static std::pair<std::string, std::vector<std::string>> parse_line_for_proxy(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line);
};
class Genotype_proxy_map {
    Genotype_proxy_status _genotype_proxy_status;
    std::vector<std::vector<std::string>> proxy_allele_matrix;
    std::vector<std::string> SNP_vec;
public: 
    //Constructor: creates empty map
    Genotype_proxy_map() :_genotype_proxy_status(Genotype_proxy_status::empty) {}
    Genotype_proxy_status status()const { return _genotype_proxy_status; }
    //Creates proxy map by reading file:
    // Does not allow reading of multiple files, as the genotype map and raw genotype
    // is only aligned by their index. 
    bool read_map(const std::string& map_filename, const Genotype_proxy_flags& genotype_proxy_flags);
    //Number of genotype fields: 
    int size()const;
    //Find SNP at index: NOTE: allele_pos_idx is 0-based, convention used by UK Biobank!
    std::pair<bool,std::string> get_SNP_name(const int allele_pos_idx)const;
    //Find proxy allele at index: NOTE: allele_type_idx is 0-based, convention used by UK Biobank!
    std::pair<bool, std::string> get_proxy_allele(const int allele_pos_idx, const int allele_type_idx)const;
};
class Genotype_file_converter {
    //Converts raw genotype file to proxy ped file used by plink --make bed.
    //Workflow:
    // 1. Constructs converter with Genotype_proxy_map and optional phenotype_map
    // 2. Checks if Genotype_file_converter is valid
    // 3-n. Converts raw genotype streams/files to proxy ped stream/files.
    Genotype_proxy_map* _gpm_ptr;//Stored as pointer. (To make the context  
    Phenotype_map* _pm_ptr;//Stored as reference.
    void constructor_postprocess();//Finishes setup after constructor is called
public:
    //Constructor: genotype and phenotype
    Genotype_file_converter(Genotype_proxy_map* genotype_proxy_map_ptr, Phenotype_map* phenotype_map_ptr = nullptr);
    bool is_valid()const;
    //convert functions: returns true if convertion is sucessful.
    //When false is returned, no gurantee that output is not modified. 
    bool convert(std::istream& is, std::ostream& os, const Genotype_subject_flags& genotype_subject_flags)const;
    bool convert(const std::string& input_filename, std::ostream& os, const Genotype_subject_flags& genotype_subject_flags)const;
    bool convert(std::istream& is, const std::string& output_filename, const Genotype_subject_flags& genotype_subject_flags)const;
    bool convert(const std::string& input_filename, const std::string& output_filename, const Genotype_subject_flags& genotype_subject_flags)const;
};
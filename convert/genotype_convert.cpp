#include "genotype_convert.h"
#include <fstream>
std::pair<int, int> Genotype_subject_line_parser::find_genotype_idx_range(const Genotype_subject_flags& genotype_subject_flags, const std::string& line){
    return find_idx_range(line, genotype_subject_flags.genotype_idx, genotype_subject_flags.delimiter);
}
std::pair<int, int> Genotype_subject_line_parser::find_UID_idx_range(const Genotype_subject_flags& genotype_subject_flags, const std::string& line){
    return find_idx_range(line, genotype_subject_flags.UID_idx, genotype_subject_flags.delimiter);
}
std::pair<std::string, std::string> Genotype_subject_line_parser::parse_line(const Genotype_subject_flags& genotype_subject_flags, const std::string& line){
    const auto genotype_range = find_genotype_idx_range(genotype_subject_flags,line);
    const auto uid_range = find_UID_idx_range(genotype_subject_flags,line);
    std::string uid_str;
    std::string genotype_str;
    if (uid_range.first < line.size() && uid_range.second > 0) {
        uid_str = line.substr(uid_range.first, uid_range.second);
    }
    if (genotype_range.first < line.size() && genotype_range.second > 0) {
        genotype_str = line.substr(genotype_range.first, genotype_range.second);
    }
    return std::make_pair(uid_str, genotype_str);
}
std::pair<int, int> Genotype_line_parser::find_SNP_idx_range(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line){
    return find_idx_range(line, genotype_proxy_flags.SNP_idx, genotype_proxy_flags.delimiter);
}
std::pair<int, int> Genotype_line_parser::find_base_idx_range(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line, const int base_choice_idx){
    return find_idx_range(line, genotype_proxy_flags.base_start_idx + base_choice_idx - 1, genotype_proxy_flags.delimiter);
}
std::pair<std::string, std::vector<std::string>> invalid_genotype_line() {
    return std::make_pair("", std::vector<std::string>());
}
std::pair<std::string, std::vector<std::string>> Genotype_line_parser::parse_line_for_original(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line){
    const auto snp_range = find_SNP_idx_range(genotype_proxy_flags,line);
    std::string snp_str;
    if(snp_range.first < line.size() && snp_range.second > 0) {
        snp_str = line.substr(snp_range.first, snp_range.second);
    }
    else {
        //Range is not valid, corresponding fields do not exist.
        return invalid_genotype_line();
    }
    std::vector<std::string> rval_second;
    for (int base_choice_idx = 1; base_choice_idx <= 4; base_choice_idx++) {
        std::string base_str;
        const auto base_range = find_base_idx_range(genotype_proxy_flags,line, base_choice_idx);
        //Removes the potential double quotation ":
        if (base_range.first >= line.size() || base_range.second <= 0) {
            return invalid_genotype_line();
        }
        int head_adjust = line[base_range.first] == '"' ? 1 : 0;
        int length_adjust = (line[base_range.first + base_range.second - 1] == '"' ? -1 : 0) - head_adjust;
        if (base_range.first +head_adjust < line.size() && base_range.second + length_adjust >= 0) {
            base_str = line.substr(base_range.first + head_adjust, base_range.second + length_adjust);
        }
        else {
            return invalid_genotype_line();
        }
        rval_second.push_back(base_str);
    }
    return std::make_pair(snp_str, rval_second);
}
std::pair<std::string, std::string> parse_base(const std::string& pair_str) {
    //Returns pair of base sequence. 
    //Three cases: empty, two consequent letters, two types separated by space.
    if (pair_str == "") {
        return std::make_pair("", "");
    }
    if (pair_str.size() == 2) {
        return std::make_pair(pair_str.substr(0, 1), pair_str.substr(1, 1));
    }
    const auto first_range = Line_parser::find_idx_range(pair_str, 1, ' ');
    const auto second_range = Line_parser::find_idx_range(pair_str, 2, ' ');
    return std::make_pair(pair_str.substr(first_range.first, first_range.second), pair_str.substr(second_range.first, second_range.second));
}
std::pair<std::string, std::vector<std::string>> Genotype_line_parser::parse_line_for_proxy(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line){
    const auto original_pair = parse_line_for_original(genotype_proxy_flags,line);
    if (original_pair.first == "") {
        return invalid_genotype_line();
    }
    const auto& pair_vec = original_pair.second;
    std::unordered_map<std::string, char> origin_proxy_map;
    //Map is distinct for each line, ordered by their apperance in the line.
    //Except that:
    //1. "0" is always mapped to 0 independent of its apperance order. 
    //2. "" should not appear in the result for later use, as "" only appears when there are less than 4 base choices.
    // We map it to "E" to indicate that there may be error. This can be used for detecting misalignment of the maps. 
    origin_proxy_map["0"] = '0';
    origin_proxy_map[""] = 'E';
    char proxy_char = '1';
    std::vector<std::string> proxy_pair_vec;
    for (const auto& base_pair_str : pair_vec) {
        const auto base_pair_pair = parse_base(base_pair_str);
        if (origin_proxy_map.count(base_pair_pair.first) == 0) {
            origin_proxy_map[base_pair_pair.first] = proxy_char++;
        }
        if (origin_proxy_map.count(base_pair_pair.second) == 0) {
            origin_proxy_map[base_pair_pair.second] = proxy_char++;
        }
        std::string proxy_pair_str;
        proxy_pair_str.push_back(origin_proxy_map[base_pair_pair.first]);
        proxy_pair_str.push_back(' ');
        proxy_pair_str.push_back(origin_proxy_map[base_pair_pair.second]);
        proxy_pair_vec.push_back(proxy_pair_str);
    }
    return std::make_pair(original_pair.first, proxy_pair_vec);
}
bool Genotype_proxy_map::read_map(const std::string& map_filename, const Genotype_proxy_flags& genotype_proxy_flags) {
    //read_map can only be called for an empty Genotype_proxy_map:
    if (status() != Genotype_proxy_status::empty) {
        return false;
    }
    //Open file for read: 
    std::fstream gt_fs;//genotype filestream
    gt_fs.open(map_filename,std::ios::in);
    if (!gt_fs.is_open()) {
        return false;
    }
    //Read lines, add to proxy_allele_matrix
    std::string cur_line;//We do not expect phenotype line to be long. 
    if (genotype_proxy_flags.skip_first_row) {
        std::getline(gt_fs, cur_line);
    }
    while (!gt_fs.eof()) {
        std::getline(gt_fs, cur_line);
        const auto parse_result = Genotype_line_parser::parse_line_for_proxy(genotype_proxy_flags, cur_line);
        if (parse_result.first == "") {
            continue;
        }
        SNP_vec.push_back(parse_result.first);
        proxy_allele_matrix.push_back(parse_result.second);
    }
    _genotype_proxy_status = Genotype_proxy_status::ready;
    return true;
}
int Genotype_proxy_map::size()const {
    if (_genotype_proxy_status != Genotype_proxy_status::ready) {
        return 0;
    }
    return SNP_vec.size();
}
std::pair<bool,std::string> Genotype_proxy_map::get_SNP_name(const int allele_pos_idx)const {
    if (_genotype_proxy_status != Genotype_proxy_status::ready || allele_pos_idx >= size()) {
        return std::make_pair(false,""); 
    }
    return std::make_pair(true, SNP_vec[allele_pos_idx]);
}
std::pair<bool,std::string> Genotype_proxy_map::get_proxy_allele(const int allele_pos_idx, const int allele_type_idx)const {
    if (_genotype_proxy_status != Genotype_proxy_status::ready || allele_pos_idx >= size()) {
        return std::make_pair(false, "");
    }
    if (allele_type_idx >= proxy_allele_matrix[allele_pos_idx].size() || proxy_allele_matrix[allele_pos_idx][allele_type_idx] == "E E") {
        return std::make_pair(false, "");
    }
    return std::make_pair(true, proxy_allele_matrix[allele_pos_idx][allele_type_idx]);
}
std::pair<bool, std::string> Genotype_proxy_map::get_proxy_allele_line(const std::string& raw_genotype_str)const {
    if (status() != Genotype_proxy_status::ready) {
        return std::make_pair(false, "");
    }
    if (raw_genotype_str.size() != size()) {
        return std::make_pair(false, "");
    }
    if (raw_genotype_str.size() == 0) {
        return std::make_pair(true, "");
    }
    std::string proxy_genotype_str;
    proxy_genotype_str.reserve(size() * 4);
    int allele_pos_idx = 0;
    for (const char ch : raw_genotype_str){
        const auto proxy_allele_pair = get_proxy_allele(allele_pos_idx++, ch - '0');
        if (proxy_allele_pair.first) {
            proxy_genotype_str.append(proxy_allele_pair.second);
            proxy_genotype_str.append(1, ' ');
        }
        else {
            return std::make_pair(false, "");//allele_type_idx out of bound.
        }
    }
    proxy_genotype_str.pop_back();//Removes extra space. 
    return std::make_pair(true, proxy_genotype_str);
}
Genotype_file_converter::Genotype_file_converter(const Genotype_proxy_map* genotype_proxy_map_ptr, const Phenotype_map* phenotype_map_ptr):
_gpm_ptr(genotype_proxy_map_ptr),_pm_ptr(phenotype_map_ptr){}
bool Genotype_file_converter::is_valid() const{
    //Checks basic validity:
    //genotype proxy map: (Required)
    if (_gpm_ptr == nullptr || _gpm_ptr->status() != Genotype_proxy_status::ready) {
        return false;
    }
    //phenotype map: (Optional)
    if (_pm_ptr && _pm_ptr->status() != Phenotype_status::ready) {
        return false;
    }
    //Further check requring information about  are not possible.
}
enum class Phenotype_map_type {unknown,omitted,discrete,scalar};
Phenotype_map_type find_phenotype_map_state(const std::string& UID, const Phenotype_map* _pm_ptr) {
    Phenotype_map_type pm_t = Phenotype_map_type::unknown;
    if (_pm_ptr == nullptr) {
        pm_t = Phenotype_map_type::omitted;
    }
    else if (_pm_ptr->find_discrete_state(UID).first) {
        pm_t = Phenotype_map_type::discrete;
    }
    else if (_pm_ptr->find_scalar_state(UID).first) {
        pm_t = Phenotype_map_type::scalar;
    }
    return pm_t;
}
bool Genotype_file_converter::convert(std::istream& is, std::ostream& os, const Genotype_subject_flags& genotype_subject_flags)const {
    //Check validity of file converter:
    if (is_valid() == false) {
        return false;
    }
    //Read lines:
    std::string cur_line;
    if (genotype_subject_flags.skip_first_row) {
        std::getline(is, cur_line);
    }
    Phenotype_map_type pm_t = Phenotype_map_type::unknown;
    while (!is.eof()) {
        std::getline(is, cur_line);
        const auto parse_result = Genotype_subject_line_parser::parse_line(genotype_subject_flags, cur_line);
        //As an empty line commonly follows a text file, we disregard this line when both UID and genotype
        // were not found. 
        if (parse_result.first == "" && parse_result.second == "") {
            continue;
        }
        //Check length of parsed raw genotype matching phenotype: 
        //Recall: first: subject UID. second: raw genotype. 
        if (parse_result.second.size() != _gpm_ptr->size()) {
            return false;
        }
        //Check subject UID exists if phenotype exists. 
        if (pm_t == Phenotype_map_type::unknown) {
            //Should only run once during convert. 
            pm_t = find_phenotype_map_state(parse_result.first, _pm_ptr);
        }
        //Prints UID: 
        os << parse_result.first << ' ';//Per plink document, space-delimited. 
        //Prints Phenotype:
        switch (pm_t)
        {
        case Phenotype_map_type::unknown:
            //Phenotype map provided, but UID not found. Convertion result is invalid. 
            return false;
            break;
        case Phenotype_map_type::omitted:
            //Optional phenotype not provided to file_converter.
            break;
        case Phenotype_map_type::discrete:{
            const auto discrete_phenotype_enquiry_result = _pm_ptr->find_discrete_state(parse_result.first);
            if (discrete_phenotype_enquiry_result.first) {
                os << discrete_phenotype_enquiry_result.second << ' ';
            }
            else {
                //Phenotype does not exist in map.
                return false;
            }
            break;
        }
        case Phenotype_map_type::scalar: {
            const auto scalar_phenotype_enquiry_result = _pm_ptr->find_scalar_state(parse_result.first);
            if (scalar_phenotype_enquiry_result.first) {
                os << scalar_phenotype_enquiry_result.second << ' ';
            }
            else {
                //Phenotype does not exist in map.
                return false;
            }
            break;
        }

        }
        //Prints proxy allele:
        const auto genotype_enquiry_result = _gpm_ptr->get_proxy_allele_line(parse_result.second);
        if (genotype_enquiry_result.first) {
            os << genotype_enquiry_result.second << std::endl;
        }
        else {
            return false;
        }
    }
    return true;
}
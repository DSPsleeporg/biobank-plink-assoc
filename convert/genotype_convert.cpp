#include "genotype_convert.h"
#include <fstream>
#include <thread>
#include <mutex>
#include <execution>
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
std::pair<int, int> Genotype_line_parser::find_chromosome_idx_range(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line) {
    return find_idx_range(line, genotype_proxy_flags.chromosome_idx, genotype_proxy_flags.delimiter);
}
std::pair<int, int> Genotype_line_parser::find_base_idx_range(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line, const int base_choice_idx){
    return find_idx_range(line, genotype_proxy_flags.base_start_idx + base_choice_idx - 1, genotype_proxy_flags.delimiter);
}
std::pair<std::string, std::vector<std::string>> invalid_genotype_line() {
    return std::make_pair("", std::vector<std::string>());
}
std::string Genotype_line_parser::parse_chromosome(const Genotype_proxy_flags& genotype_proxy_flags, const std::string& line) {
    const auto idx_range = find_chromosome_idx_range(genotype_proxy_flags, line);
    std::string rval;
    if (idx_range.first < line.size() && idx_range.second > 0) {
        rval = line.substr(idx_range.first, idx_range.second);
    }
    else {
        rval = "0";//plink uses "0" as "unplaced"
    }
    return rval;
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
        if (base_str.size() > 0) {
            rval_second.push_back(base_str);
        }
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
        if (proxy_pair_str != "E E") {
            //Only adds valid allele pair. 
            proxy_pair_vec.push_back(proxy_pair_str);
        }
        
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
        std::vector<int> proxy_allele_matrix_row;
        for (const std::string& allele_pair_str : parse_result.second) {
            char allele_pair_cstr[4];
            std::strcpy(allele_pair_cstr, allele_pair_str.c_str());
            proxy_allele_matrix_row.push_back(*static_cast<int*>(static_cast<void *>(allele_pair_cstr)));
        }
        proxy_allele_matrix.push_back(proxy_allele_matrix_row);
        //Read chromosome: (When chromosome not provided, it defaults to 0, as defined by the Genotype_line_parser
        chromosome_vec.push_back(Genotype_line_parser::parse_chromosome(genotype_proxy_flags, cur_line));
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
std::ostream& Genotype_proxy_map::print(std::ostream& os)const {
    //Recall: plink map format: 4 column space-separated
    //Chromosome, SNP, genetic distance, base-pair position (we use the index).
    if (status() != Genotype_proxy_status::ready) {
        return os;
    }
    for (int i = 0; i < size(); i++) {
        os << get_chromosome_name(i).second << ' ' << get_SNP_name(i).second << " 0 " <<
            i+1 << '\n';
    }
    return os;
}
bool Genotype_proxy_map::print(const std::string& filename)const {
    std::ofstream pm_fs;
    pm_fs.open(filename, std::ios::trunc);
    if (!pm_fs.is_open()) {
        return false;
    }
    print(pm_fs);
    return true;
}
std::pair<bool,std::string> Genotype_proxy_map::get_SNP_name(const int allele_pos_idx)const {
    if (_genotype_proxy_status != Genotype_proxy_status::ready || allele_pos_idx >= size()) {
        return std::make_pair(false,""); 
    }
    return std::make_pair(true, SNP_vec[allele_pos_idx]);
}
std::pair<bool, std::string> Genotype_proxy_map::get_chromosome_name(const int allele_pos_idx)const {
    if (_genotype_proxy_status != Genotype_proxy_status::ready || allele_pos_idx >= size()) {
        return std::make_pair(false, "");
    }
    return std::make_pair(true, chromosome_vec[allele_pos_idx]);
}
std::pair<bool, int> Genotype_proxy_map::get_allele_count(const int allele_pos_idx)const {
    if (_genotype_proxy_status != Genotype_proxy_status::ready || allele_pos_idx >= size()) {
        return std::make_pair(false, 0);
    }
    else {
        return std::make_pair(true, proxy_allele_matrix[allele_pos_idx].size());
    }
}
inline void Genotype_proxy_map::_get_proxy_allele(const int allele_pos_idx, const int allele_type_idx, char& base1, char& base2)const {
    const int allele_pair_int =  proxy_allele_matrix[allele_pos_idx][allele_type_idx];
    const char* char_ptr = static_cast<const char*>(static_cast<const void*>(&allele_pair_int));
    base1 = *char_ptr;
    base2 = *(char_ptr + 2);
    return;
}
std::pair<bool,std::string> Genotype_proxy_map::get_proxy_allele(const int allele_pos_idx, const int allele_type_idx)const {
    if (_genotype_proxy_status != Genotype_proxy_status::ready) {
        return std::make_pair(false, "");
    }
    if (allele_type_idx >= proxy_allele_matrix[allele_pos_idx].size()) {
        return std::make_pair(false, "");
    }
    std::string proxy_allele_str;
    char base1;
    char base2;
    _get_proxy_allele(allele_pos_idx, allele_type_idx, base1, base2);
    proxy_allele_str.append(1, base1);
    proxy_allele_str.append(1, ' ');
    proxy_allele_str.append(1, base2);
    return std::make_pair(true, proxy_allele_str);
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
        if (get_allele_count(allele_pos_idx).second <= ch - '0') {
            return std::make_pair(false, "");
        }
        char base1;
        char base2;
        _get_proxy_allele(allele_pos_idx++, ch - '0', base1, base2);
        proxy_genotype_str.append(1, base1);
        proxy_genotype_str.append(1, ' ');
        proxy_genotype_str.append(1, base2);
        proxy_genotype_str.append(1, ' ');
        //const auto proxy_allele_pair = get_proxy_allele(allele_pos_idx++, ch - '0');
        //proxy_genotype_str.append(proxy_allele_pair.second);
        //proxy_genotype_str.append(1, ' ');
    }
    proxy_genotype_str.pop_back();//Removes extra space. 
    return std::make_pair(true, proxy_genotype_str);
}
Genotype_file_converter::Genotype_file_converter(const Genotype_proxy_map* genotype_proxy_map_ptr, const Phenotype_map* phenotype_map_ptr):
_gpm_ptr(genotype_proxy_map_ptr),_pm_ptr(phenotype_map_ptr){}
Genotype_file_convert_status Genotype_file_converter::is_valid() const{
    //Checks basic validity:
    //genotype proxy map: (Required)
    if (_gpm_ptr == nullptr || _gpm_ptr->status() != Genotype_proxy_status::ready) {
        return Genotype_file_convert_status::genotype_map_fail;
    }
    //phenotype map: (Optional)
    if (_pm_ptr && _pm_ptr->status() != Phenotype_status::ready) {
        return Genotype_file_convert_status::phenotype_map_fail;
    }
    //Further check requring information about  are not possible.
    return Genotype_file_convert_status::success;
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
Genotype_file_convert_status convert_file_from_lines(std::string& out_lines, const Phenotype_map_type pm_t, std::vector<std::pair<std::string, std::string>>& parse_result_vector, const Phenotype_map* _pm_ptr, const Genotype_proxy_map* _gpm_ptr) {
    Genotype_file_convert_status rval = Genotype_file_convert_status::success;
    std::mutex str_mutex;
    std::mutex rval_mutex;
    std::for_each(std::execution::par_unseq, parse_result_vector.begin(), parse_result_vector.end(), [&](const std::pair<std::string, std::string>& parse_result) {
        //Prints UID: 
        //os << parse_result.first << ' ';//Per plink document, space-delimited. 
        std::string out_line;
        out_line.append(parse_result.first);
        out_line.append(1, ' ');
        //Prints Phenotype:
        switch (pm_t)
        {
        case Phenotype_map_type::unknown:
        case Phenotype_map_type::omitted:
            //Optional phenotype not provided to file_converter.
            break;
        case Phenotype_map_type::discrete: {
            const auto discrete_phenotype_enquiry_result = _pm_ptr->find_discrete_state(parse_result.first);
            if (discrete_phenotype_enquiry_result.first) {
                //os << discrete_phenotype_enquiry_result.second << ' ';
                out_line.append(std::to_string(discrete_phenotype_enquiry_result.second));
                out_line.append(1, ' ');
            }
            else {
                //Phenotype does not exist in map.
                std::lock_guard<std::mutex> lock(rval_mutex);
                rval =  Genotype_file_convert_status::phenotype_miss_fail;
                return;
            }
            break;
        }
        case Phenotype_map_type::scalar: {
            const auto scalar_phenotype_enquiry_result = _pm_ptr->find_scalar_state(parse_result.first);
            if (scalar_phenotype_enquiry_result.first) {
                //os << scalar_phenotype_enquiry_result.second << ' ';
                out_line.append(std::to_string(scalar_phenotype_enquiry_result.second));
                out_line.append(1, ' ');
            }
            else {
                //Phenotype does not exist in map.
                std::lock_guard<std::mutex> lock(rval_mutex);
                rval =  Genotype_file_convert_status::phenotype_miss_fail;
                return;
            }
            break;
        }

        }
        //Prints proxy allele:
        const auto genotype_enquiry_result = _gpm_ptr->get_proxy_allele_line(parse_result.second);
        if (genotype_enquiry_result.first) {
            //os << genotype_enquiry_result.second << '\n';
            out_line.append(genotype_enquiry_result.second);
            out_line.append(1, '\n');
        }
        else {
            std::lock_guard<std::mutex> lock(rval_mutex);
            rval =  Genotype_file_convert_status::genotype_file_invalid_fail;
            return;
        }
        std::lock_guard<std::mutex> lock(str_mutex);
        out_lines.append(out_line);
        });
    parse_result_vector.resize(0);
    return rval;
}
Genotype_file_convert_status Genotype_file_converter::convert(std::istream& is, std::ostream& os, const Genotype_subject_flags& genotype_subject_flags)const {
    //Check validity of file converter:
    if (is_valid() != Genotype_file_convert_status::success) {
        return is_valid();
    }
    //Read lines:
    std::string cur_line;
    if (genotype_subject_flags.skip_first_row) {
        std::getline(is, cur_line);
    }
    Phenotype_map_type pm_t = Phenotype_map_type::unknown;
    std::vector<std::pair<std::string,std::string>> parse_result_vector;
    const int parse_max_size = 32;
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
            return Genotype_file_convert_status::size_mismatch_fail;
        }
        //Check subject UID exists if phenotype exists. 
        if (pm_t == Phenotype_map_type::unknown) {
            //Should only run once during convert. 
            pm_t = find_phenotype_map_state(parse_result.first, _pm_ptr);
            if (pm_t == Phenotype_map_type::unknown) {
                return Genotype_file_convert_status::UID_miss_fail;
            }
        }
        parse_result_vector.push_back(parse_result);
        if (parse_result_vector.size() < parse_max_size) {
            continue;
        }
        else {
            std::string out_lines;
            Genotype_file_convert_status genotype_file_convert_status;
            genotype_file_convert_status = convert_file_from_lines(out_lines, pm_t, parse_result_vector, _pm_ptr, _gpm_ptr);
            if (genotype_file_convert_status == Genotype_file_convert_status::success) {
                os << out_lines;
            }
            else {
                return genotype_file_convert_status;
            }
        }
    }
    //Clean remaining entries:
    std::string out_lines;
    Genotype_file_convert_status genotype_file_convert_status;
    genotype_file_convert_status = convert_file_from_lines(out_lines, pm_t, parse_result_vector, _pm_ptr, _gpm_ptr);
    if (genotype_file_convert_status == Genotype_file_convert_status::success) {
        os << out_lines;
    }
    else {
        return genotype_file_convert_status;
    }
    return Genotype_file_convert_status::success;
}
Genotype_file_convert_status Genotype_file_converter::convert(const std::string& input_filename, std::ostream& os, const Genotype_subject_flags& genotype_subject_flags)const {
    std::fstream gt_fs;//genotype filestream
    gt_fs.open(input_filename, std::ios::in);
    if (!gt_fs.is_open()) {
        return Genotype_file_convert_status::input_file_fail;
    }
    else {
        return convert((std::istream&)gt_fs, os, genotype_subject_flags);
    }
}
Genotype_file_convert_status Genotype_file_converter::convert(std::istream& is, const std::string& output_filename, const Genotype_subject_flags& genotype_subject_flags)const {
    std::fstream gt_fs;//genotype filestream
    gt_fs.open(output_filename, std::ios::out);
    if (!gt_fs.is_open()) {
        return Genotype_file_convert_status::output_file_fail;
    }
    else {
        return convert(is, (std::ostream&)gt_fs, genotype_subject_flags);
    }
}
Genotype_file_convert_status Genotype_file_converter::convert(const std::string& input_filename, const std::string& output_filename, const Genotype_subject_flags& genotype_subject_flags)const {
    std::fstream gt_fs;//genotype filestream
    gt_fs.open(output_filename, std::ios::out);
    if (!gt_fs.is_open()) {
        return Genotype_file_convert_status::output_file_fail;
    }
    else {
        return convert(input_filename, (std::ostream&)gt_fs, genotype_subject_flags);
    }
}

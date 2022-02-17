#include "phenotype_convert.h"
#include <fstream>
std::pair<int,int> Phenotype_flags::find_idx_range(const std::string& line, const int field_idx)const{
    const int N = line.size();
    int delimiter_count = 0;
    int i = 0;
    for (;i<N;i++){
        //Find field_idx-1 delimiters: 
        if (delimiter_count >= field_idx-1){
            break;
        }
        if (line[i] == delimiter){
            delimiter_count++;
            continue;
        }
    }
    int start_idx = i;
    int count = 0;
    //Read in current field:
    for (;i<N;i++){
        if (line[i] == delimiter){
            break;
        }
        else{
            count++;
        }
    }
    return std::make_pair(start_idx,count);
}
std::pair<int,int> Phenotype_flags::find_phenotype_idx_range(const std::string& line)const{
    return find_idx_range(line,phenotype_idx);
}
std::pair<int,int> Phenotype_flags::find_UID_idx_range(const std::string& line)const{
    return find_idx_range(line,UID_idx);
}
std::pair<std::string,std::string> Phenotype_flags::parse_line(const std::string& line) const{
    const auto phenotype_range = find_phenotype_idx_range(line);
    const auto uid_range = find_UID_idx_range(line);
    std::string uid_str;
    std::string phenotype_str;
    if (uid_range.first < line.size() && uid_range.second > 0){
        uid_str = line.substr(uid_range.first,uid_range.second);
    }
    if (phenotype_range.first < line.size() && phenotype_range.second > 0){
        phenotype_str = line.substr(phenotype_range.first,phenotype_range.second);
    }
    return std::make_pair(uid_str,phenotype_str);
}
bool Scalar_phenotype_map::read_phenotype(const Phenotype_flags& phenotype_flags){
    //Check that this map is in an state that allows read: For the scalar case, it is always possible, 
    // unless the file or the map is already corrupted. 
    if (_phenotype_status == Phenotype_status::spoiled){
        return false;
    }
    //Open file for read: 
    std::fstream pt_fs;//phenotype filestream
    pt_fs.open(phenotype_flags.filename,std::ios::in);
    if (!pt_fs.is_open()){
        return false;
    }
    //Read lines, and add to uid_raw_map:
    std::string cur_line;//We do not expect phenotype line to be long. 
    if (phenotype_flags.skip_first_row) {
        std::getline(pt_fs, cur_line);
    }
    while (!pt_fs.eof()){
        std::getline(pt_fs,cur_line);
        const auto parse_result = phenotype_flags.parse_line(cur_line);
        //As an empty line commonly follows a text file, we disregard this line when both UID and phenotype
        // were not found. 
        if (parse_result.first == "" && parse_result.second == ""){
            continue;
        }
        auto map_itr = uid_scalar_map.find(parse_result.first);
        if (map_itr != uid_scalar_map.end()){
            //Such entry already exists. As UID should be unique, we assume the data is corrputed. 
            _phenotype_status = Phenotype_status::spoiled;
            return false;
        }
        else{
            //Assign phenotype to UID map. 
            uid_scalar_map[parse_result.first] = std::stod(parse_result.second);
        }
    }
    _phenotype_status = Phenotype_status::ready;
    return true;
}
Phenotype_status Scalar_phenotype_map::status()const{
    return _phenotype_status;
}
int Scalar_phenotype_map::size()const {
    return uid_scalar_map.size();
}
std::ostream& Scalar_phenotype_map::print(std::ostream& os)const{
    //Per plink phenotype definition: 
    //3 columns: family ID, individual ID (UID), phenotype:
    if (status() != Phenotype_status::ready){
        //Only print when the map is in ready state:
        return os;
    }
    //Iterate through all individuals, output into stream: 
    for (auto uid_scalar_pair : uid_scalar_map){
        os << uid_scalar_pair.first << " " << uid_scalar_pair.first << " " << uid_scalar_pair.second << std::endl;
    }
    return os;
}
std::pair<bool,double> Scalar_phenotype_map::find_scalar_state(const std::string& uid_str)const{
    if (status() != Phenotype_status::ready){
        return std::make_pair(false,0.0);
    }
    auto map_itr = uid_scalar_map.find(uid_str);
    if (map_itr == uid_scalar_map.end()){
        //Not found.
        return std::make_pair(false,0.0);
    }
    else{
        return std::make_pair(true,map_itr->second);
    }
}
std::pair<bool,int> Scalar_phenotype_map::find_discrete_state(const std::string& uid_str)const{
    //Finding discrete state is not possible. 
    return std::make_pair(false,0);
}
bool Discrete_phenotype_map::read_phenotype(const Phenotype_flags& phenotype_flags){
    //Check that this map is in an state that allows read: For discrete case, only empty and bisect_wait permits new input.
    if (_phenotype_status != Phenotype_status::bisect_wait && _phenotype_status != Phenotype_status::empty){
        return false;
    }
    //Open file for read: 
    std::fstream pt_fs;//phenotype filestream
    pt_fs.open(phenotype_flags.filename,std::ios::in);
    if (!pt_fs.is_open()){
        return false;
    }
    //Read lines, and add to uid_raw_map:
    std::string cur_line;//We do not expect phenotype line to be long. 
    if (phenotype_flags.skip_first_row) {
        std::getline(pt_fs, cur_line);
    }
    while (!pt_fs.eof()){
        std::getline(pt_fs,cur_line);
        const auto parse_result = phenotype_flags.parse_line(cur_line);
        //As an empty line commonly follows a text file, we disregard this line when both UID and phenotype
        // were not found. 
        if (parse_result.first == "" && parse_result.second == ""){
            continue;
        }
        auto map_itr = uid_raw_map.find(parse_result.first);
        if (map_itr != uid_raw_map.end()){
            //Such entry already exists. As UID should be unique, we assume the data is corrputed. 
            _phenotype_status = Phenotype_status::spoiled;
            return false;
        }
        else{
            //Assign phenotype to UID map. 
            uid_raw_map[parse_result.first] = parse_result.second;
        }
    }
    _phenotype_status = Phenotype_status::bisect_wait;
    return true;
}
Phenotype_status Discrete_phenotype_map::status()const{
    return _phenotype_status;
}
int Discrete_phenotype_map::size()const {
    return uid_raw_map.size();
}
std::ostream& Discrete_phenotype_map::print(std::ostream& os)const{
    //Per plink phenotype definition: 
    //3 columns: family ID, individual ID (UID), phenotype:
    if (status() != Phenotype_status::ready){
        //Only print when the map is in ready state:
        return os;
    }
    //Iterate through all individuals, output into stream: 
    for (auto uid_raw_pair : uid_raw_map){
        os << uid_raw_pair.first << " " << uid_raw_pair.first << " " << raw_bisect_map.at(uid_raw_pair.second) << std::endl;
    }
    return os;
}
std::pair<bool,double> Discrete_phenotype_map::find_scalar_state(const std::string& uid_str)const{
    return std::make_pair(false,0.0);
}
std::pair<bool,int> Discrete_phenotype_map::find_discrete_state(const std::string& uid_str)const{
    if (_phenotype_status != Phenotype_status::ready){
        return std::make_pair(false,0);
    }
    auto map_itr = uid_raw_map.find(uid_str);
    if (map_itr == uid_raw_map.end()){
        //Not found.
        return std::make_pair(false,0);
    }
    else{
        return std::make_pair(true,raw_bisect_map.at(map_itr->second));
    }
}
std::unordered_set<std::string> Discrete_phenotype_map::get_raw_phenotype()const {
    std::unordered_set<std::string> rval;
    for (const auto& uid_raw_pair : uid_raw_map) {
        rval.insert(uid_raw_pair.second);
    }
    return rval;
}
Phenotype_status Discrete_phenotype_map::set_phenotype_map(const std::unordered_set<std::string>& positive_phenotypes) {
    //Only allows setting if the system is in bisect_wait or ready state.
    if (status() != Phenotype_status::bisect_wait && status() != Phenotype_status::ready) {
        //As the map is not properly set, this map may not match the assumption from the place calling it. 
        _phenotype_status = Phenotype_status::spoiled;
        return status();
    }
    //Start from a fresh map. 
    raw_bisect_map = std::unordered_map<std::string, int>();
    std::unordered_set<std::string> raw_phenotype_set = get_raw_phenotype();
    //Check if positive_phenotypes is a subset of raw_phenotype_set: 
    for (const std::string& str : positive_phenotypes) {
        auto rps_itr = raw_phenotype_set.find(str);
        if (rps_itr == raw_phenotype_set.end()) {
            //Contains non-existent flag, assuming positive set not valid. 
            _phenotype_status = Phenotype_status::spoiled;
            return status();
        }
        else {
            raw_bisect_map[str] = 1;
            raw_phenotype_set.erase(rps_itr);
        }
    }
    //Remaining raw phenotypes are negative (0):
    for (const std::string& str : raw_phenotype_set) {
        raw_bisect_map[str] = 0;
    }
    _phenotype_status = Phenotype_status::ready;
    return status();
}
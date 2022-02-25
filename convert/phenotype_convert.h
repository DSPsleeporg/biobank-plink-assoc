//This header file defines the phenotype mapping methods.
//We assume the input is of the following form: 
//... field-1 UID field1 field2... phenotype_field...
//(phenotype can also be front of UID)
//where UID is a string that uniquely identifies the subject. 
// The index of phenotype_field can change for each instance of the program, 
// but must be constant for the current instance.
// Delimiter can be replaced 
//A typical procedure is as follows: 
//1. User specifies the format of the input phenotype file: 
// delimiter, index of phenotype field, is the phenotype discrete or a scalar.
//2. The program reads all the phenotypes, and creates a set of phenotypes.
//3. User bisects the phenotype set
//4. Phenotype is constructed. It is then ready for enquiry using uid.
//(5.) The result can be exported to a plink-compatiple ASCII phenotype file.
#pragma once
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <utility>
enum class Phenotype_status{empty,bisect_wait,ready,spoiled};
class Line_parser {
public:
    static std::pair<int, int> find_idx_range(const std::string& line, const int field_idx, const char delimiter);
};
struct Phenotype_flags {
    char delimiter;
    int UID_idx;//1-based, consistent with convention used by plink
    int phenotype_idx;//1-based, consistent with convention used by plink
    bool is_discrete;
    bool skip_first_row;
};
class Phenotype_line_parser : private Line_parser{
public:
    //Highlights phenotype_field: (Reserved for GUI features) Returns start_idx,count pair
    static std::pair<int,int> find_phenotype_idx_range(const Phenotype_flags& phenotype_flags, const std::string& line);
    //Highlights UID field: (Reserved for GUI features) Returns start_idx,count pair
    static std::pair<int,int> find_UID_idx_range(const Phenotype_flags& phenotype_flags, const std::string& line);
    //Returns pair of UID and phenotype
    static std::pair<std::string,std::string> parse_line(const Phenotype_flags& phenotype_flags, const std::string& line);
};
class Phenotype_map{
public:
    //reads phenotype file. Return true if read is successful.
    //Allows multiple input files, but would refuse to read once map of raw_phenotype to binary phenotype is established.
    virtual bool read_phenotype(const std::string& filename, const Phenotype_flags& phenotype_flags) = 0;
    //Return status of the Phenotype_map:
    virtual Phenotype_status status()const = 0;
    //Number of subjects:
    virtual int size()const = 0;
    //Print result to basic_ostream (console or file stream)
    virtual std::ostream& print(std::ostream& os)const = 0;
    //Enquire phenotype state of UID. 
    // First entry returns whether the enquiry is successful. Second returns the value
    //Scalar version: 
    virtual std::pair<bool,double> find_scalar_state(const std::string& uid_str)const = 0;
    //Discrete version: 
    virtual std::pair<bool,int> find_discrete_state(const std::string& uid_str)const = 0;
    virtual ~Phenotype_map(){};
};
class Scalar_phenotype_map : public Phenotype_map{
private:
    Phenotype_status _phenotype_status;
    std::unordered_map<std::string,double> uid_scalar_map;
public: 
    //Constructor: creates empty map
    Scalar_phenotype_map():_phenotype_status(Phenotype_status::empty){}
    //Inherited methods: 
    bool read_phenotype(const std::string& filename, const Phenotype_flags& phenotype_flags);
    //Return status of the Phenotype_map:
    Phenotype_status status()const;
    int size()const;
    //Print result to basic_ostream (console or file stream)
    std::ostream& print(std::ostream& os)const;
    //Enquire phenotype state of UID. 
    // First entry returns whether the enquiry is successful. Second returns the value
    //Scalar version: 
    std::pair<bool,double> find_scalar_state(const std::string& uid_str)const;
    //Discrete version: 
    std::pair<bool,int> find_discrete_state(const std::string& uid_str)const;
};
class Discrete_phenotype_map : public Phenotype_map{
private:
    Phenotype_status _phenotype_status;
    std::unordered_map<std::string,std::string> uid_raw_map;//Maps uid to raw phenotype
    std::unordered_map<std::string,int> raw_bisect_map;//Maps raw phenotype to bisected group, represented by 0 and 1.
public:
    //Constructor: creates empty map
    Discrete_phenotype_map():_phenotype_status(Phenotype_status::empty){}
    //Inherited methods: 
    bool read_phenotype(const std::string& filename, const Phenotype_flags& phenotype_flags);
    //Return status of the Phenotype_map:
    Phenotype_status status()const;
    int size()const;
    //Print result to basic_ostream (console or file stream)
    std::ostream& print(std::ostream& os)const;
    //after reading file, returns the set of raw phenotypes:
    //Enquire phenotype state of UID. 
    // First entry returns whether the enquiry is successful. Second returns the value
    //Scalar version: 
    std::pair<bool,double> find_scalar_state(const std::string& uid_str)const;
    //Discrete version: 
    std::pair<bool,int> find_discrete_state(const std::string& uid_str)const;

    //Discrete_phenotype-specific methods: 
    //Find the set of all states: 
    std::unordered_set<std::string> get_raw_phenotype()const;
    //Sets the mapping from raw phenotype to binary states: return status of map.
    Phenotype_status set_phenotype_map(const std::unordered_set<std::string>& positive_phenotypes);
};
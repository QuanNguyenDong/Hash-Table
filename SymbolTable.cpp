#include "SymbolTable.h"
#define SPACE ' '
#define GLOBAL_SCOPE 0

void process_command(string& str, const int& num, const string& cmd, string arr[]) {
    int pos, i=0;
    if (str[str.size()-1] == SPACE) { throw(InvalidInstruction(cmd)); }
    while (i <= num) {
        if (str[0] == SPACE) { throw(InvalidInstruction(cmd)); }
        pos = str.find(SPACE);
        arr[i] = str.substr(0,pos);
        if (str.find(SPACE)+1 == 0) break;
        str = str.substr(pos+1);
        i++;
    }
}

bool is_number_constant(string& str) {
    std::string::const_iterator it = str.begin();
    if (*it == '-') it++;
    while (it != str.end() && std::isdigit(*it)) it++;
    return !str.empty() && it == str.end();
}

long long encode(const string& str, const int& lvl) {
    string tmp = to_string(lvl);
    int encd = 0;
    for (unsigned int i=0; i < str.size(); i++) {
        encd = (int)str[i] - 48;
        tmp.append(to_string(encd));
    }
    return stoll(tmp, nullptr, 10);
}

bool is_string_constant(string& str) {
    int len = str.size();
    if (str[0] != '\'') { return false; }
    if (str[len-1] != '\'') { return false; }
    for (int i=1; i<len-2; i++) {
        if (str[i] >= '0' && str[i] <= '9') {}
        else if (str[i] >= 'a' && str[i] <= 'z') {}
        else if (str[i] >= 'A' && str[i] <= 'Z') {}
        else if (str[i] == ' ') {}
        else {
            return false;
        }
    }
    return true;
}

bool is_ID_name(string& str) {
    if (str[0] < 'a') return false;
    if (str[0] > 'z') return false;
    int len = str.length();
    for (int i=1; i<len; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {}
        else if (str[i] >= 'A' && str[i] <= 'Z') {}
        else if (str[i] == '_') {}
        else if (str[i] >= '0' && str[i] <= '9') {}
        else return false;
    }
    return true;
}

bool SymbolTable::is_valid_argument(string str, string cmd, const int& scp, int& slot, HashNode* funct) {
    int pos = str.find('(');
    string tmp;

    // parameters
    str = str.substr(pos+1, str.size()-pos-2);
    pos = str.find(',');
    
    int i = 0;                  // number of para
    int* ar = funct->args;      // arr holds datatype

    // check every argument passing
    while (pos != -1 || i == 0) {
        if (i >= funct->parameters-1 && i != 0) {
            throw(TypeMismatch(cmd));
        }

        tmp = str.substr(0, pos);
        if (pos != -1) str = str.substr(pos+1, str.size()-pos);
        pos = str.find(',');

        if (ar[i] == -1) {
            if (is_number_constant(tmp)) ar[i] = 0;
            else if (is_string_constant(tmp)) ar[i] = 1;
            else if (is_ID_name(tmp)) {
                int j=scp;
                HashNode* temp = NULL;
                for (; j>=0; j--) {
                    temp = this->hash_ptr->get(encode(tmp, j), slot);
                    if (temp != NULL) break;
                }
                if (temp == NULL) {
                    throw(Undeclared(tmp));
                }
                else if (temp->parameters != -1) {
                    throw(TypeMismatch(cmd));
                }

                j = temp->type;
                if (j == -1) {
                    throw(TypeCannotBeInferred(cmd));
                }
                else {
                    ar[i] = j;
                }
            }
            else {
                throw(InvalidInstruction(cmd));
            }
        }
        else if (ar[i] == 0 && is_number_constant(tmp)) {}
        else if (ar[i] == 1 && is_string_constant(tmp)) {}
        else if (is_ID_name(tmp)) {
            if (is_number_constant(tmp)) ar[i] = 0;
            else if (is_string_constant(tmp)) ar[i] = 1;
            else if (is_ID_name(tmp)) {
                int j=scp;
                HashNode* temp = NULL;
                for (; j>=0; j--) {
                    temp = this->hash_ptr->get(encode(tmp, j), slot);
                    if (temp != NULL) break;
                }
                if (temp == NULL) {
                    throw(Undeclared(tmp));
                }
                else if (temp->parameters != -1) {
                    throw(TypeMismatch(cmd));
                }

                j = temp->type;
                if (j == -1) {
                    throw(TypeCannotBeInferred(cmd));
                }
                else if (ar[i] != j) {
                    throw(TypeMismatch(cmd));
                }
            }
            else {
                throw(InvalidInstruction(cmd));
            }
        }
        i++;
    }
    if (i < funct->parameters-1) {
        throw(InvalidInstruction(cmd));
    }
    
    return true;
}


bool isNumeic(const string& str) {
    for (unsigned int i=0; i < str.size(); i++) {
        if (str[i] < '0' || str[i] > '9') { return false; }
    }
    return true;
}

string tokenize(string& str) {
    int pos = str.find(SPACE);
    return str.substr(0, pos);
}

void SymbolTable::run(string filename)
{
    ifstream file;
    string cmd, opt, ar[5], str;
    int scope = 0, m, c;
    file.open(filename);

    getline(file, cmd);
    opt = tokenize(cmd);
    str = cmd;

    // Declare Hash table
    if (opt == "LINEAR") {

        process_command(str, 3, cmd, &ar[0]);

        if (ar[0].empty() || ar[1].empty() || ar[2].empty() || !ar[3].empty()) { throw(InvalidInstruction(cmd)); }
        else if (!isNumeic(ar[1]) || !isNumeic(ar[2])) { throw(InvalidInstruction(cmd)); }
        
        m = stoi(ar[1]);
        c = stoi(ar[2]);
        this->hash_ptr = new LinearHashing(m, c);
    }
    else if (opt == "QUADRATIC") {

        int c1, c2;

        process_command(str, 4, cmd, &ar[0]);

        if (ar[0].empty() || ar[1].empty() || ar[2].empty() || ar[3].empty() || !ar[4].empty()) { throw(InvalidInstruction(cmd)); }
        else if (!isNumeic(ar[1]) || !isNumeic(ar[2]) || !isNumeic(ar[3])) { throw(InvalidInstruction(cmd)); }

        m = stoi(ar[1]);
        c1 = stoi(ar[2]);
        c2 = stoi(ar[3]);
        this->hash_ptr = new QuadraticHashing(m, c1, c2);
    }
    else if (opt == "DOUBLE") {

        process_command(str, 3, cmd, &ar[0]);

        if (ar[0].empty() || ar[1].empty() || ar[2].empty() || !ar[3].empty()) { throw(InvalidInstruction(cmd)); }
        else if (!isNumeic(ar[1]) || !isNumeic(ar[2])) { throw(InvalidInstruction(cmd)); }

        m = stoi(ar[1]);
        c = stoi(ar[2]);
        this->hash_ptr = new DoubleHashing(m, c);
    } 
    else {
        throw(InvalidInstruction(cmd));
    }

    while (!file.eof()) {
        getline(file, cmd);
        opt = tokenize(cmd);
        str = cmd;

        string id, arr[4];
        int slot=0;

        if (opt == "INSERT") {
            int para;

            process_command(str, 3, cmd, &arr[0]);
            if (arr[1].empty() || !is_ID_name(arr[1]) || !arr[3].empty()) { throw(InvalidInstruction(cmd)); }
            id = arr[1];

            if (!arr[2].empty()) {
                if (!isNumeic(arr[2])) { throw(InvalidInstruction(cmd)); }
                else {
                    if (scope == 0) {
                        para = stoi(arr[2]);
                    }
                    else {
                        throw(InvalidDeclaration(id));
                    }
                }

                if (scope != 0) { throw(InvalidInstruction(cmd)); }
                this->hash_ptr->insertNode(encode(arr[1],scope), arr[1], scope, slot, cmd, para);
            } 
            else {
                this->hash_ptr->insertNode(encode(arr[1],scope), arr[1], scope, slot, cmd);
            }
            cout << slot << endl;    
        }
        else if (opt == "ASSIGN") {
            string value;
            int i=scope, asgn = -1;
            
            process_command(str, 3, cmd, &arr[0]);

            if (arr[1].empty() || arr[2].empty() || !arr[3].empty()) { throw(InvalidInstruction(cmd)); }
            id = arr[1];
            value = arr[2];

            // for function node
            HashNode* func = NULL;
            // for another id node
            HashNode* var_tmp = NULL;

            // check value first -> identifier name latter
            if (is_number_constant(value)) { asgn = 0; }   
            else if (is_string_constant(value)) { asgn = 1; }
            else if (is_ID_name(value)) {
                i = scope;
                while (i >= 0) {    // find another id node
                    var_tmp = this->hash_ptr->get(encode(value, i), slot);
                    if (var_tmp != NULL) {
                        break;
                    }
                    i--;
                }

                if (var_tmp == NULL) {
                    throw(Undeclared(value));
                }
                else if (var_tmp->parameters != -1) {
                    throw(TypeMismatch(cmd));
                }
                else {
                    func = var_tmp;
                }
            }
            else if (value[value.size() - 1] == ')') {

                int pn = value.find('(');
                if (pn < 0) {
                    throw(TypeMismatch(cmd));
                }
                else if (value[pn+1] == ')') {  // no arguments passing
                    i = scope;
                    while (i >= 0) {
                        func = this->hash_ptr->get(encode(value.substr(0, pn), i), slot);
                        if (func != NULL) break;
                        i--;
                    }
                    if (func == NULL) {
                        throw(Undeclared(value.substr(0, pn)));
                    }
                    else if (func->parameters != 0) {
                        throw(TypeMismatch(cmd));
                    }
                }
                else {
                    i = scope;
                    while (i >= 0) {
                        func = this->hash_ptr->get(encode(value.substr(0, pn), i), slot);
                        if (func != NULL) break;
                        i--;
                    }
                    if (func == NULL) {
                        throw(Undeclared(value.substr(0, pn)));
                    }

                    // check args passing
                    is_valid_argument(value, cmd, scope, slot, func);
                }
            }
            else {
                throw(Undeclared(id));
            }
            
            // find identifier name being assigned
            HashNode* identifier_name = NULL;
            i = scope;
            while (i >= 0) {
                identifier_name = this->hash_ptr->get(encode(id, i), slot);
                if (identifier_name != NULL) break;
                i--;
            }
            if (identifier_name == NULL) {
                throw(Undeclared(id));
            }
            else if (identifier_name->parameters != -1) {
                throw(TypeMismatch(cmd));
            }
            
            if (func != NULL) { // handle type compatible
                if (func->type == -1 && identifier_name->type == -1) {
                    throw(TypeCannotBeInferred(cmd));
                }
                else if (func->type == -1) {
                    func->type = identifier_name->type;
                }
                else if (identifier_name->type == -1) {
                    identifier_name->type = func->type;
                }
                else if (identifier_name->type == func->type) {}
                else {
                    throw(TypeMismatch(cmd));
                }
            }
            else if (identifier_name->type == -1) {
                identifier_name->type = asgn;
            }
            else if (identifier_name->type == asgn) {}
            else {
                throw(TypeMismatch(cmd));
            }

            cout << slot << endl;
        }
        else if (opt == "CALL") {
            if (cmd[cmd.size() - 1] != ')') {
                throw(InvalidInstruction(cmd));
            }

            process_command(str, 2, cmd, &arr[0]);

            if (arr[1].empty() || !arr[2].empty() || arr[1][arr[1].size()-1] != ')') { throw(InvalidInstruction(cmd)); }
            string value = arr[1];

            int i = scope, pn = value.find('(');
            HashNode* func = NULL;
            while (i >= 0) {
                func = this->hash_ptr->get(encode(value.substr(0, pn), i), slot);
                if (func != NULL) break;
                i--;
            }
            if (func == NULL) {
                throw(Undeclared(value.substr(0, pn)));
            }
            else if (func->parameters == -1) {
                throw(TypeMismatch(cmd));
            }

            if (is_valid_argument(value, cmd, scope, slot, func)) {
                cout << slot << endl;
            }
        }
        else if (opt == "BEGIN") {
            int pos = cmd.find(SPACE);
            if (pos != -1) { throw(InvalidInstruction(cmd)); }
            
            scope++;
        }
        else if (opt == "END") {
            int pos = cmd.find(SPACE);
            if (pos != -1) { throw(InvalidInstruction(cmd)); }
            
            if (scope <= 0) {
                throw(UnknownBlock());
            }

            this->hash_ptr->deleteScope(scope);
            scope--;
        }
        else if (opt == "LOOKUP") {
            int tmp = scope;

            process_command(str, 2, cmd, &arr[0]);
            id = arr[1];

            if (id.empty() || !arr[2].empty() || !is_ID_name(id)) { throw(InvalidInstruction(cmd)); }

            while (tmp >= 0) {
                slot = this->hash_ptr->get_pos(encode(id, tmp));
                if (slot != -1) { break; }
                tmp--;
            }

            if (slot == -1) {
                throw(Undeclared(id));
            }
            else {
                cout << slot << endl;
            }
        }
        else if (opt == "PRINT") {
            int pos = cmd.find(SPACE);
            if (pos != -1) { throw(InvalidInstruction(cmd)); }            
            
            this->hash_ptr->print();
        }
        else {
            throw(InvalidInstruction(cmd));
        }
    }

    if (scope != 0) {
        throw(UnclosedBlock(scope));
    }
}
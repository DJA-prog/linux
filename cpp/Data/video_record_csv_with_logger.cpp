// id name date_mod length height width tags category format location portrait

// includes
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
string command, id, file_path;
class Logger{ // require: none

    public:
        Logger(string logFile, string title)
        {
            int append_name = 1;
            while (log_exists(logFile))
            {
                logFile = logFile + '.' + to_string(append_name);
                append_name++;
            }
            
            log_file.open(logFile, std::fstream::app);
            if (!log_file.is_open())
                cout << "Could not create / open log file: " + logFile << endl;
            log_file << "Log: " << title << endl;
        }

        void append(string data)
        {
            log_file << data;
        }  
        void append(char data)
        {
            log_file << data;
        } 

        void appendCode(int code)
        {
            if (code == 0)
            {
                log_file << "[ SUCCESS ]";
            }
            else
            {
                log_file << "[ FAILED ]";
            }
            
        }

        void endLine()
        {
            log_file << endl;
        }

        void logClose()
        {
            log_file.close();
        }

    private:
        fstream log_file;
        bool log_exists(string path)
        {
            fstream myfile;
            myfile.open(path, ios::in);
            if (myfile.is_open())
            {
                myfile.close();
                return true;
            }
            return false;    
        }

};

class CSVWriter{ // require: none

    public:
        CSVWriter(string csvFile)
        {
            int append_name = 1;
            while (csv_exists(csvFile))
            {
                csvFile = csvFile + '.' + to_string(append_name);
                append_name++;
            }
            
            csv_file.open(csvFile, std::fstream::app);
            if (!csv_file.is_open())
                cout << "Could not create / open log file: " + csvFile << endl;
        }

        void appendField(string data)
        {
            if (newLine)
            {
                csv_file << data;
            }
            else{
                csv_file << ',' << data;
            }
            newLine = false;
        }  

        void appendField(char data)
        {
            if (newLine)
            {
                csv_file << data;
            }
            else{
                csv_file << ',' << data;
            }
            newLine = false;
        } 

        void appendField(int data)
        {
            if (newLine)
            {
                csv_file << data;
            }
            else{
                csv_file << ',' << data;
            }
            newLine = false;
        }

        void endRow()
        {
            csv_file << endl;
            newLine = true;
        }

        void csvClose()
        {
            csv_file.close();
        }

    private:
        bool newLine = true;
        fstream csv_file;
        bool csv_exists(string path)
        {
            fstream myfile;
            myfile.open(path, ios::in);
            if (myfile.is_open())
            {
                myfile.close();
                return true;
            }
            return false;    
        }

};

void sessionId(string prefix) // require: none
{
    srand(time(0));
    id = prefix + to_string(rand());
}

int removeTmp(string path) // require: none
{
    command = "rm " + path;
    return system(command.c_str());
}

int digitsInInt(int n) // require: none
{
    int number_of_digits = 0;
    do {
        ++number_of_digits; 
        n /= 10;
    } while (n);
    return number_of_digits;
}

string zeros(int value, int digits) // require: digitsInInt()
{
    string output = to_string(value);
    int digitCount = digitsInInt(value);
    for (size_t i = 0; i < digits - digitCount; i++)
    {
        output = '0' + output;
    }
    return output;
}

string getFileLine(string path, Logger& log) // require: none
{
    log.append("\tgetFileLine:");log.endLine();
    fstream index_file;
    string file_content_line;
    index_file.open(path, ios::in);
    log.append("\t\tRetrieve Line: ");
    if (index_file.is_open()){ 
      	getline(index_file, file_content_line);
      	index_file.close();
        log.appendCode(0);log.endLine();
   	}else{
   		cout << "could not open "<< path << endl;
        log.appendCode(1);log.endLine();
   	}
    return file_content_line;
}

vector<string> getFileLines(string path, Logger& log) // require: none
{
    log.append("\tgetFileLines:");log.endLine();
    fstream index_file;
    string file_content_line;
    vector<string> list;
    index_file.open(path, ios::in);
    log.append("\t\tRetrieve Line(s): ");
    if (index_file.is_open()){ 
      	while(getline(index_file, file_content_line)){
            list.push_back(file_content_line);
      	}
      	index_file.close();
        log.appendCode(0);log.endLine();
   	}else{
   		cout << "could not open "<< path << endl;
        log.appendCode(1);log.endLine();
   	}
    return list;
}

string getVideoDuration(string path, string sessionId, Logger& log) // require: zeros() getFileLine() removeTmp
{
    log.append("\tgetVideoDuration:");log.endLine();
    string tmp_file = "/tmp/" + sessionId + "_vidDur";
    command = "ffprobe -i \'" + path + "\' -v quiet -show_entries format=duration -hide_banner -of default=noprint_wrappers=1:nokey=1 >> " + tmp_file;
    log.append("\t\tffprobe duration: "); log.appendCode(system(command.c_str()));log.endLine();
    int duration = (int) stoi(getFileLine(tmp_file, log));
    int minutes = duration / 60;
    int hours = minutes / 60;
    int seconds = duration % 60;
    string duration_str = zeros(hours, 2) + ':' + zeros(minutes, 2) + ':' + zeros(seconds, 2);
    log.append("\t\tDuration: " + duration_str);log.endLine();
    log.append("\t\tRemove tmp: "); log.appendCode(removeTmp(tmp_file));log.endLine();
    return duration_str;
}

vector<int> getVideoDimentions(string path, string sessionId, Logger& log) // height, width  // require: removeTmp() getFileLine()
{
    log.append("\tgetVideoDimentions:");log.endLine();
    vector<int> dim;
    string tmp_file = "/tmp/" + sessionId + "_vidDim";
    command = "ffprobe -i \'" + path + "\' -v error -select_streams v:0 -show_entries stream=height -of csv=s=x:p=0 > " + tmp_file;
    log.append("\t\tffprobe height: "); log.appendCode(system(command.c_str()));log.endLine();
    dim.push_back(stoi(getFileLine(tmp_file,log)));
    command = "ffprobe -i \'" + path + "\' -v error -select_streams v:0 -show_entries stream=width -of csv=s=x:p=0 > " + tmp_file;
    log.append("\t\tffprobe width: "); log.appendCode(system(command.c_str()));log.endLine();
    dim.push_back(stoi(getFileLine(tmp_file,log)));

    log.append("\t\tHeight: " + to_string(dim[0]));log.endLine();
    log.append("\t\tWidth: " + to_string(dim[1]));log.endLine();
    log.append("\t\tRemove tmp: "); log.appendCode(removeTmp(tmp_file));log.endLine();
    return dim;
}

string getLastMod(string path, string sessionId, Logger& log) // require: removeTmp() getFileLine()
{
    log.append("\tgetLastMod:");log.endLine();
    string tmp_file = "/tmp/" + sessionId + "_Lastmod";
    command = "stat \'" + path + "\' | grep Modify > " + tmp_file;
    log.append("\t\tStat execute: "); log.appendCode(system(command.c_str()));log.endLine();
    string mod_datetime = getFileLine(tmp_file,log);
    log.append("\t\tmod_datetime: " + mod_datetime);log.endLine();
    string mod_date = mod_datetime.substr(8, 10);
    log.append("\t\tRemove tmp: "); log.appendCode(removeTmp(tmp_file));log.endLine();
    return mod_date;
}

string pathinfo(string url, int file_info) // 0:filepath ; 1:filename ; 2:basename ; 3: file_extention ; 4: parent_directory  // require: none
{
    int dot_find;
    int url_length = url.length();
    string filepath, filename, basename, extention, parent_dir;
    if (url.rfind('.') == string::npos)
    {
        dot_find = url_length;
        extention = "";
    }else{
        dot_find = url.rfind('.');
        extention = url.substr(dot_find +1, url_length - dot_find -1);
    }

    if (url.rfind('/') == string::npos)
    {
        basename = url.substr(0 , dot_find);
        filename = url;
        filepath = ".";
    }else{
        basename = url.substr(url.rfind('/') +1, dot_find - url.rfind('/')-1);
        filename = url.substr(url.rfind('/') +1, url_length - url.rfind('/')-1);
        filepath = url.substr(0, url.rfind('/'));
        parent_dir = url.substr(url.rfind('/', url.rfind('/') - 1) + 1, url.rfind('/') - url.rfind('/', url.rfind('/') - 1) - 1);
    }

    switch (file_info)
    {
    case 0:
        return filepath;
        break;
    
    case 1:
        return filename;
        break;

    case 2:
        return basename;
        break;

    case 3:
        return extention;
        break;

    default:
        return parent_dir;
        break;
    }
}

vector<string> getDirList(string path, string sessionId, Logger& log) // require: removeTmp() getFileLines()
{
    string tmp_file = sessionId + "_dirList";
    command = "ls \'" + path + "\' > " + tmp_file;
    system(command.c_str());
    vector<string> files = getFileLines(tmp_file, log);
    removeTmp(tmp_file);
    return files;
}

vector<string> getFileData(string file_path, Logger& log) // require: pathinfo() getLastMod() getVideoDuration() getVideoDimentions()
{
    vector<string> returndata;
    string name = pathinfo(file_path, 2); // name
    string date_mod = getLastMod(file_path, id, log); // date_mod
    string length = getVideoDuration(file_path, id, log); // length
    int height = getVideoDimentions(file_path, id, log)[0]; // height
    int width = getVideoDimentions(file_path, id, log)[1]; // width
    string tags = ""; // tags
    string category = pathinfo(file_path, 4); // category
    string format = pathinfo(file_path, 3); // format
    string location = file_path; // location
    bool portrait; if (height > width){portrait = true;}else{portrait = false;} // portrait

    returndata.push_back(name);
    returndata.push_back(date_mod);
    returndata.push_back(length);
    returndata.push_back(to_string(height));
    returndata.push_back(to_string(width));
    returndata.push_back(tags);
    returndata.push_back(category);
    returndata.push_back(format);
    returndata.push_back(location);
    returndata.push_back(to_string(portrait));

    return returndata;
}

int main(int argc, char const *argv[]) // path_to_parent_directory 0-sub 1-current
{
    // generate id for tmp files
    sessionId("vidRecDB_");
    Logger logging("vidLog", "Video Record writen to CSV file");
    logging.append("SessionID: " + id);logging.endLine();

    CSVWriter csv("vid.csv");

    string path  = argv[1];
    if (path.rfind('/') != path.length() - 1)
    {
        path += '/';
    }
    // cout << path << endl;
    string current_location = argv[2];
    vector<string> subDirList;

    if (current_location == "0")
    {
        // cout << "data in sub folders" << endl;
        subDirList = getDirList(path, id, logging);
        for (string a: subDirList)
        {
            // cout << "Sub folder: " << a << endl;
            for (string b: getDirList(path + a, id, logging))
            {
                // cout << "file: " << path + a + '/' + b << endl;
                if (b != "thumb")
                {
                    cout << "Data: ";
                    logging.append("####Data: ");
                    for (string c: getFileData(path + a + '/' + b, logging))
                    {
                        cout << c << " : ";
                        logging.append(c + " : ");
                        csv.appendField(c);
                    }
                    csv.endRow();
                    cout << endl;
                    logging.endLine();
                }
            }
        }
    }
    else
    {
        // cout << "data in parent folder" << endl;
        for (string b: getDirList(path, id, logging))
            {
                // cout << "file: " << path + a + '/' + b << endl;
                if (b != "thumb")
                {
                    cout << "Data: ";
                    logging.append("####Data: ");
                    for (string c: getFileData(path + '/' + b, logging))
                    {
                        cout << c << " : ";
                        logging.append(c + " : ");
                        csv.appendField(c);
                    }
                    csv.endRow();
                    cout << endl;
                    logging.endLine();
                }
            }
    }
    
    logging.logClose();
    csv.csvClose();
    return 0;
}

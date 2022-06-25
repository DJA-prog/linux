// id name date_mod length height width tags category format location portrait

// includes
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
string command, id, file_path;

void sessionId(string prefix)
{
    srand(time(0));
    id = prefix + to_string(rand());
}

void removeTmp(string path)
{
    command = "rm " + path;
    system(command.c_str());
}

int digitsInInt(int n)
{
    int number_of_digits = 0;
    do {
        ++number_of_digits; 
        n /= 10;
    } while (n);
    return number_of_digits;
}

string zeros(int value, int digits)
{
    string output = to_string(value);
    int digitCount = digitsInInt(value);
    for (size_t i = 0; i < digits - digitCount; i++)
    {
        output = '0' + output;
    }
    return output;
}

string getFileLine(string path)
{
    fstream index_file;
    string file_content_line;
    index_file.open(path, ios::in);
    if (index_file.is_open()){ 
      	getline(index_file, file_content_line);
      	index_file.close();
   	}else{
   		cout << "could not open "<< path << endl;
   	}
    return file_content_line;
}

vector<string> getFileLines(string path)
{
    fstream index_file;
    string file_content_line;
    vector<string> list;
    index_file.open(path, ios::in);
    if (index_file.is_open()){ 
      	while(getline(index_file, file_content_line)){
            list.push_back(file_content_line);
      	}
      	index_file.close();
   	}else{
   		cout << "could not open "<< path << endl;
   	}
    return list;
}

string getVideoDuration(string path, string sessionId)
{
    string tmp_file = "/tmp/" + sessionId + "_vidDur";
    command = "ffprobe -i \'" + path + "\' -v quiet -show_entries format=duration -hide_banner -of default=noprint_wrappers=1:nokey=1 >> " + tmp_file;
    // cout << command << endl;
    system(command.c_str());
    int duration = (int) stoi(getFileLine(tmp_file));
    int minutes = duration / 60;
    int hours = minutes / 60;
    int seconds = duration % 60;
    string duration_str = zeros(hours, 2) + ':' + zeros(minutes, 2) + ':' + zeros(seconds, 2);
    removeTmp(tmp_file);
    return duration_str;
}

vector<int> getVideoDimentions(string path, string sessionId) // height, width
{
    vector<int> dim;
    string tmp_file = "/tmp/" + sessionId + "_vidDim";
    command = "ffprobe -i \'" + path + "\' -v error -select_streams v:0 -show_entries stream=height -of csv=s=x:p=0 > " + tmp_file;
    // cout << command << endl;
    system(command.c_str());
    dim.push_back(stoi(getFileLine(tmp_file)));
    command = "ffprobe -i \'" + path + "\' -v error -select_streams v:0 -show_entries stream=width -of csv=s=x:p=0 > " + tmp_file;
    // cout << command << endl;
    system(command.c_str());
    dim.push_back(stoi(getFileLine(tmp_file)));
    
    removeTmp(tmp_file);
    return dim;
}

string getLastMod(string path, string sessionId)
{
    string tmp_file = "/tmp/" + sessionId + "_Lastmod";
    command = "stat \'" + path + "\' | grep Modify > " + tmp_file;
    system(command.c_str());
    string mod_datetime = getFileLine(tmp_file);
    string mod_date = mod_datetime.substr(8, 10);
    removeTmp(tmp_file);
    return mod_date;
}

string pathinfo(string url, int file_info) // 0:filepath ; 1:filename ; 2:basename ; 3: file_extention ; 4: parent_directory
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

vector<string> getDirList(string path, string sessionId)
{
    string tmp_file = sessionId + "_dirList";
    command = "ls \'" + path + "\' > " + tmp_file;
    system(command.c_str());
    vector<string> files = getFileLines(tmp_file);
    removeTmp(tmp_file);
    return files;
}

vector<string> getFileData(string file_path)
{
    vector<string> returndata;
    string name = pathinfo(file_path, 2); // name
    string date_mod = getLastMod(file_path, id); // date_mod
    string length = getVideoDuration(file_path, id); // length
    int height = getVideoDimentions(file_path, id)[0]; // height
    int width = getVideoDimentions(file_path, id)[1]; // width
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
        subDirList = getDirList(path, id);
        for (string a: subDirList)
        {
            // cout << "Sub folder: " << a << endl;
            for (string b: getDirList(path + a, id))
            {
                // cout << "file: " << path + a + '/' + b << endl;
                if (b != "thumb")
                {
                    cout << "Data: ";
                    for (string c: getFileData(path + a + '/' + b))
                        cout << c << " : ";
                    cout << endl;
                }
            }
        }
    }
    else
    {
        // cout << "data in parent folder" << endl;
        for (string b: getDirList(path, id))
        {
            cout << "Data: ";
            for (string c: getFileData(path + b))
                cout << c << " : ";
            cout << endl;
        }
    }
    
    
    


    return 0;
}

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

string critical = "false"; // true or false

string path = "D:\\Workspace\\Osu To PJSK";

struct note{
        float start;
        float duration;
        int pos;
};

string onedec(float n) {
        float w = round(n * 10.0) / 10.0;
        string q = to_string(w);
        size_t decimal_pos = q.find('.');
        if (decimal_pos != std::string::npos) {
            q = q.substr(0, decimal_pos + 2); // Include one decimal point
        }
        return q;
}

int main() {
        float bpm;
        float mspb;
        string filename;

        cout << "File Name: ";
        getline(cin, filename);

        ifstream file(path + "\\" + filename + ".txt");

        if (!file.is_open()) {
                cout << "FILE NOT FOUND!" << endl;
                return 0;
        }

        string line;        

        while(getline(file, line) and line != "[TimingPoints]") {
                continue;
        }

        getline(file, line);
        int n = 0;
        string ms = "0.";
        while (line[n] != ',') n++; n++;
        while (line[n] != ',') {
                if (line[n] != '.') ms +=  line[n];
                n++;
        }
        mspb = stof(ms);
        bpm = 60 / mspb;

        while(getline(file, line) and line != "[HitObjects]") {
                continue;
        }

        cout << "BPM: " << bpm << endl;
        cout << "Ms per Beat: " << mspb << endl;
        vector<note> notes;
        while(getline(file, line)) {
                note temp;
                int n = 0;
                string position;
                string time;
                string type;
                string dur;
                while (line[n] != ',') {
                        position += line[n];
                        n++;
                }
                n++;
                while (line[n] != ',') {
                        n++;
                }
                n++;
                while (line[n] != ',') {
                        time += line[n];
                        n++;
                }
                n++;
                while (line[n] != ',') {
                        type += line[n];
                        n++;
                }
                n++;
                while (line[n] != ',') {
                        n++;
                }
                n++;
                while (line[n] != ',') {
                        dur += line[n];
                        n++;
                }

                temp.pos = (stoi(position) - 64) / 128;
                temp.start = stof(time) / (mspb * 1000);
                if (type == "1") temp.duration = 0;
                else if (type == "128") temp.duration = stof(dur) / (mspb * 1000) - temp.start;
                if (type == "5") temp.duration = 0;

                notes.push_back(temp);
        }
        file.close();

        cout << "CONVERSTION COMPLETED!" << endl;

        ofstream ofile(filename + "_pjsk.txt");

        string newbpm = to_string(bpm);
        size_t decimal_pos = newbpm.find('.');
        if (decimal_pos != std::string::npos) {
            newbpm = newbpm.substr(0, decimal_pos + 2); // Include one decimal point
        }

        string output = "{\"usc\":{\"objects\":[{\"beat\":0.0,\"bpm\":" + newbpm + ",\"type\":\"bpm\"},{\"changes\":[{\"beat\":0.0,\"timeScale\":1.0}],\"type\":\"timeScaleGroup\"},";

        for (int i = 0; i < notes.size(); i++) {
                string nl;
                if (notes[i].duration == 0) {   // TAP

                        nl = "{\"beat\":" + onedec(notes[i].start) + ",\"critical\":" + critical + ",\"lane\":" + onedec(-4.5 + (notes[i].pos * 3)) + ",\"size\":1.5,\"timeScaleGroup\":0,\"trace\":false,\"type\":\"single\"}";

                        if (i != notes.size() - 1) nl += ",";

                        output += nl;
                } else {        // HOLD
                        nl = "{\"connections\":[{\"beat\":" + onedec(notes[i].start) + ",\"critical\":" + critical + ",\"ease\":\"linear\",\"judgeType\":\"normal\",\"lane\":" + onedec(-4.5 + (notes[i].pos * 3)) + ",\"size\":1.5,\"timeScaleGroup\":0,\"type\":\"start\"},{\"beat\":" 
                        + onedec(notes[i].start + notes[i].duration) + ",\"critical\":" + critical + ",\"judgeType\":\"normal\",\"lane\":" + onedec(-4.5 + (notes[i].pos * 3)) + ",\"size\":1.5,\"timeScaleGroup\":0,\"type\":\"end\"}],\"critical\":" + critical + ",\"type\":\"slide\"}";

                        if (i != notes.size() - 1) nl += ",";

                        output += nl;
                }
        }

        output += "],\"offset\":-0.0},\"version\":2}";

        cout << "OUTPUT COMPLETED!" << endl;
        ofile << output;

        ofile.close();
        
        return 0;
}

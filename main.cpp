#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

bool noHold = false;    // Remove all hold notes
bool allCrit = false; // Change all note to crit
int critrate = 0;
string flick = "false"; // true or false
int flickrate = 0;
string endtype = "none"; // normal or trace or none
bool centerCrit = true;   // If keyCount is Odd, the middle key will be yellow

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

string twodec(float n) {
        float w = round(n * 100.0) / 100.0;
        string q = to_string(w);
        size_t decimal_pos = q.find('.');
        if (decimal_pos != std::string::npos) {
            q = q.substr(0, decimal_pos + 3); // Include one decimal point
        }
        return q;
}

int main() {
        float bpm;
        float mspb;
        float keySize;
        int keyCount;
        string l;

        string filename;
        ofstream debug("debug.txt");

        cout << "File Name: ";
        getline(cin, filename);

        ifstream file(path + "\\" + filename + ".txt");

        if (!file.is_open()) {
                cout << "FILE NOT FOUND!" << endl;
                return 0;
        }

        cout << "Key Count: ";
        cin >> keyCount;

        keySize = 6.0 / (keyCount * 1.0);

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

        debug << "BPM: " << to_string(bpm) << " Ms Per Beat: " << to_string(mspb) << endl;
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
                while (line[n] != ',' and line[n] != ':') {
                        dur += line[n];
                        n++;
                }
                
                temp.pos = (stoi(position) - (256 / keyCount)) / (256 / keyCount * 2);
                temp.start = stof(twodec(stof(time) / (mspb * 1000)));
                if (type == "1") temp.duration = 0;
                else if (type == "128") temp.duration = stof(dur) / (mspb * 1000) - temp.start;
                if (type == "5") temp.duration = 0;

                debug << time << " " << position << " | " << temp.start << " " << temp.pos <<endl;
                notes.push_back(temp);
        }
        file.close();

        cout << "CONVERSTION COMPLETED!" << endl;

        ofstream ofile(filename + "_pjsk.usc");

        string newbpm = to_string(bpm);
        size_t decimal_pos = newbpm.find('.');
        if (decimal_pos != std::string::npos) {
            newbpm = newbpm.substr(0, decimal_pos + 2); // Include one decimal point
        }

        string output = "{\"usc\":{\"objects\":[{\"beat\":0.0,\"bpm\":" + newbpm + ",\"type\":\"bpm\"},{\"changes\":[{\"beat\":0.0,\"timeScale\":1.0}],\"type\":\"timeScaleGroup\"},";

        float startPos;
        if (keyCount % 2 == 0) {
                startPos = 0 - (keySize * (keyCount / 2) * 2 + (keySize)) + (keySize * 2);
        } else {
                startPos = 0 - (keySize * (keyCount / 2) * 2);
        }

        string critical;
        for (int i = 0; i < notes.size(); i++) {
                string nl;
                if (allCrit) critical = "true";
                else critical = "false";
                if(centerCrit) {
                        if (!allCrit) {
                                if (notes[i].pos == (keyCount / 2) and keyCount % 2 != 0) {
                                        critical = "true";
                                }
                        }
                }
                if (notes[i].duration == 0 or noHold) {   // TAP

                        nl = "{\"beat\":" + twodec(notes[i].start) + ",\"critical\":" + critical + ",\"lane\":" + twodec(startPos + (notes[i].pos * keySize * 2)) + ",\"size\":" + to_string(keySize) + ",\"timeScaleGroup\":0,\"trace\":false,\"type\":\"single\"}";

                        if (i != notes.size() - 1) nl += ",";

                        output += nl;
                } else {        // HOLD
                        nl = "{\"connections\":[{\"beat\":" + twodec(notes[i].start) + ",\"critical\":" + critical + ",\"ease\":\"linear\",\"judgeType\":\"normal\",\"lane\":" + twodec(startPos + (notes[i].pos * keySize * 2)) + ",\"size\":" + to_string(keySize) + ",\"timeScaleGroup\":0,\"type\":\"start\"},{\"beat\":" 
                        + twodec(notes[i].start + notes[i].duration) + ",\"critical\":" + critical + ",\"judgeType\":\"" + endtype + "\",\"lane\":" + twodec(startPos + (notes[i].pos * keySize * 2)) + ",\"size\":" + to_string(keySize) + ",\"timeScaleGroup\":0,\"type\":\"end\"}],\"critical\":" + critical + ",\"type\":\"slide\"}";

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

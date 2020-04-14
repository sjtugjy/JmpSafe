#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

int ret_cnt = 0;
int call_cnt = 0;

// Currently, check each ret and mark each after-call location
// TODO: check indirect jump and indirect call (some function entry will be legal)
int main(int argc, char **argv)
{
	if (argc != 3) {
		cout << "Usage: ./instrument input.s ouput.s" << endl;
		return -1;
	}

	fstream fs, ofs;
	fs.open(argv[1], ios::in | ios::out);
	ofs.open(argv[2], ios::in | ios::out | ios::trunc);

	int continue_cnt = 0;
	#define LINE_SZ 256
	char buffer[LINE_SZ];
	while (!fs.eof()) {
		fs.getline(buffer, LINE_SZ);

		string origin = string(buffer);
		string code = string(buffer);

		/* Eliminate the comment */
		std::size_t pos = code.find("#");	
		if (pos != string::npos) {
			code = code.substr(0, code.length() - pos - 1);
		}

		if (code.find("ret") != string::npos) {

			/* Trim the space at the front */
			code.erase(0, code.find_first_not_of(" \t"));
			/* Make sure this line is a ret instruction */
			if ((code.find("ret") != 0) && (code.find("rep ret") != 0)) {
				ofs << origin << endl;
				cout << "origin: " << origin << endl;
				cout << "code: " << code << endl;
				continue;
			}

			/* Locate the ret instruction (ret/retq) */
			ret_cnt += 1;
			ofs << "\tpopq %r10 # tmac" << endl;
			ofs << "\tmovq (%r10), %r11" << endl;
			/* Tag nopl: 0f 1f 84 80 a2 5d 4f 00 */
			ofs << "\tmov $0x4f5da280841f0f, %rdi" << endl;
			ofs << "\tcmpq %rdi, %r11" << endl;
			// ofs << "\tcmpl $0x80841f0f, %r11d" << endl;
			ofs << "\tje continue" << continue_cnt << endl;
			ofs << "\tjmp ." << endl;
			ofs << "continue" << continue_cnt << ":" << endl;
			ofs << "\tjmp *%r10 # tmac" << endl;
			continue_cnt += 1;
		} else if (code.find("call") != string::npos) {

			/* Trim the space at the front */
			code.erase(0, code.find_first_not_of(" \t"));
			/* Make sure this line is a call instruction */
			if (code.find("call") != 0) {
				ofs << origin << endl;
				cout << "origin: " << origin << endl;
				cout << "code: " << code << endl;
				continue;
			}

			call_cnt += 1;
			ofs << origin << endl;
			/* Mark each after-call location as a legal target */
			ofs << "\tnopl 5201314(%rax, %rax, 4)" << " # tmac nopq tag" << endl;

			/* Check if this is an indirect call: call(q) (*)%rxx */

			if (regex_match (code, regex("(call)(q)?( |\\t)+(\\*)?(%)(r|R)(.*)"))) {
				cout << "[Warning] find an indirect call: ";
				cout << origin << endl;
				continue;

			}

		} else if (code.find("jmp") != string::npos) {

			/* Trim the space at the front */
			code.erase(0, code.find_first_not_of(" \t"));
			/* Make sure this line is a jmp instruction */
			if (code.find("jmp") != 0) {
				ofs << origin << endl;
				cout << "origin: " << origin << endl;
				cout << "code: " << code << endl;
				continue;
			}

			ofs << origin << endl;

			int find = 0;

			/* for cases like jmp %rxx */
			if (regex_match (code, regex("(jmp)(( |\\t)+)(%)(r|R)(.*)"))) {
				find = 1;
			}

			/* Both workable */
			#if 0
			if (regex_match (code, regex("(jmp)(( |\\t)+)(\\*%)(r|R)(.*)"))) {
				cout << "match!" << endl;
			}
			#else
			code.erase(0, code.find_first_not_of("jmp \t"));
			/* Check if it is an indirect jump, i.e., jmp *%rxx */
			if ((code[0] == '*') && (code[1] == '%') &&
			    ((code[2] == 'r') || code[2] == 'R')) {
				find = 1;
			}
			#endif

			if (find == 1) {
				cout << "[Warning] find an indirect jmp: ";
				cout << origin << endl;
				continue;
			}

		} else {
			ofs << origin << endl;
		}
	}

	fs.close();
	ofs.close();

	cout << "Total: ret_cnt is " << ret_cnt << endl;
	cout << "Total: call_cnt is " << call_cnt << endl;

	return 0;
}

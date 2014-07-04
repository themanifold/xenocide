#include <string>
#include <vector>

using namespace std;

class CTutorial {
private:
	vector <string> tutorial_texts;
	size_t index;
public:
	CTutorial();
	void show_next();
};
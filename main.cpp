#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <json/json.h>
#include <json/value.h>
#include <json/writer.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>    

#include "RenderWindow.h"





void renderGame();
std :: string out;
bool copy_button_pressed = false;
bool stats_screen = false;
char* incor = strdup("⬛");
char* cor = strdup("🟩");
char* in = strdup("🟨");

char* game_name = strdup("Sesordle");

bool stat_delay = false;

int width = 1300, height = 800;
RenderWindow window(game_name, width, height);


bool debug = false;
int guess = 1;


bool win = false;
bool lose = false;

int x_off = 12;
int y_off = 12;
int start_day = 113;



int getPosition(const char *array, char c) {
	for (int i = 0; i < 6; i++)
	{
		if (array[i] == c)
		{
			return i;
		}
	}
	return -1;
     
}

int getLen(int n[][7]){
	int len = 0;
	for(int i = 0; i < 7; i++){
			if(n[i][0] != 3){
				len++;
			}
		}
	return len;
}


int getDay() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%j", &tstruct);
	int _day = atoi(buf);
	return _day;
}


int calculatePercentage(int total , int correct){
	if (total == 0){
		return 0.0f;
	}
	int percentage = ((float)correct/(float)total)*100;
	return percentage;
}


int getHour() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%H", &tstruct);
	int hour = atoi(buf);
	return hour;
}

int day = 0;
bool played = false;
int max_streak = 0;
std :: string create_output (int n[7][7], int day, int _g, bool lose) {
	std:: string _start;
	_start += game_name;
	_start += " ";
	_start += std::to_string(day);
	_start += " ";
	if (!lose) {
	_start += std::to_string(_g);
	}
	else {
		_start += "X";
	}
	_start += "/6\n";
	out = _start;
	for (int i = 0; i< getLen(n); i++) {
		for (int j = 0; j < 6; j++) {
			if (n[i][j] == 1) {
				out += in;
			}
			else if (n[i][j] == 2) {
				out += cor;
			}
			else {
				out += incor;
			}
		}
		if (i != getLen(n) - 1) {
			out += "\n";
		}

	}
	return out;
}





int guess_acc[7][7] = {
	{3},
	{3},
	{3},
	{3},
	{3},
	{3},
	{3}
};

char *guesses[7] = {}; 
int streak = 0;
int play_count = 0;
int guess_1 = 0;
int guess_2 = 0;
int guess_3 = 0;
int guess_4 = 0;
int guess_5 = 0;
int guess_6 = 0;
int per = 0;

void saveData();
void ResetGame();


void importData () {
    std::ifstream data_file("/usr/local/Hexordle/assets/data.json", std::ifstream::binary);
    Json::Value dat;
    Json::Reader reader;
    std :: string temp;
    reader.parse(data_file, dat);
    day = dat["day"].asInt();
    played = dat["played"].asBool();
    win = dat["win"].asBool();
    lose = dat["lose"].asBool();
	streak = dat["streak"].asInt();
	play_count = dat["played_count"].asInt();
	guess_1 = dat["guess_1"].asInt();
	guess_2 = dat["guess_2"].asInt();
	guess_3 = dat["guess_3"].asInt();
	guess_4 = dat["guess_4"].asInt();
	guess_5 = dat["guess_5"].asInt();
	guess_6 = dat["guess_6"].asInt();
	per = dat["percent"].asInt();
	max_streak = dat["max_streak"].asInt();
	
	if (debug) {
	std :: cout << "Data: " << dat << std :: endl;
	}

	if (getHour() >= 0 && getDay() -(start_day+1) >= day) {
		if (getDay() -(start_day+1) == day) {
		day = getDay() - start_day;
		}
		else if ( getDay() -(start_day+1) > day) {
			if (streak > max_streak) {
			max_streak = streak;
			}
			streak = 0;
		}
		ResetGame();
		saveData();
	 } 

    // importing guess data if played
    if (played) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            guess_acc[i][j] = dat["played_data"][i][j].asInt();
        }
    }
	guess = getLen(guess_acc)+1;
    for (int k = 0;k<getLen(guess_acc);k++) {
        for (int l = 0;l<6;l++) {
            temp += dat["played_letter"][k][l].asString();
        }
        guesses[k] = strdup(temp.c_str());
        temp = "";
        }
    }

}


void saveData () {
    Json::Value _dat;
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";
    std::unique_ptr<Json::StreamWriter> writer(
    builder.newStreamWriter());
    _dat["day"] = day;
    _dat["played"] = played;
    _dat["win"] = win;
    _dat["lose"] = lose;
	_dat["streak"] = streak;
	_dat["played_count"] = play_count;
	_dat["guess_1"] = guess_1;
	_dat["guess_2"] = guess_2;
	_dat["guess_3"] = guess_3;
	_dat["guess_4"] = guess_4;
	_dat["guess_5"] = guess_5;
	_dat["guess_6"] = guess_6;
	_dat["percent"] = per;
	_dat["max_streak"] = max_streak;

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            _dat["played_data"][i][j] = guess_acc[i][j];
        }
    }
	if (_dat["played_letter"].isNull()) {
		_dat["played_letter"] = Json::Value(Json::arrayValue);
	}
    for (int k = 0;k<getLen(guess_acc);k++) {
        for (int l = 0;l<6;l++) {
            std :: string tem = guesses[k];
            char c = tem[l];
            std :: string x(1, c);
            _dat["played_letter"][k][l] = x;
        }
    }

    std::ofstream _data("/usr/local/Hexordle/assets/data.json");
    writer->write(_dat, &_data);
}



void ResetGame () {
	played = false;
	win = false;
	lose = false;
	guess = 1;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			guess_acc[i][j] = 3;
		}
	}
	for (int k = 0;k<getLen(guess_acc);k++) {
		guesses[k] = strdup("\0");
	}
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			if (j == 0) {
				guess_acc[i][j] = 3;
			}
			else {
				guess_acc[i][j] = 0;
			}
		}
	}
}


char* final_word = strdup("");

void getWord () {
    std::string words;
    char* wordList[183] = {strdup("TROMIE"),strdup("PSEUDO"), strdup("PEPPER"), strdup("CUCKOO"), strdup("UNWELL"), strdup("FILTER"), strdup("PASTIE"), strdup("DINNER"), strdup("GENTRY"), strdup("FORGET"), strdup("ACROSS"), strdup("OUTLET"), strdup("COLONY"), strdup("ABROAD"), strdup("JUSTLY"), strdup("PRESTO"), strdup("DECADE"), strdup("MINNOW"), strdup("TAUGHT"), strdup("UNISON"), strdup("ROCKET"), strdup("DRINKS"), strdup("TOMATO"), strdup("INDENT"), strdup("DEMAND"), strdup("STEAMY"), strdup("AMUSER"), strdup("PAVING"), strdup("SINGLE"), strdup("LIABLE"), strdup("DAMNED"), strdup("SIGNAL"), strdup("SHABBY"), strdup("MONTHS"), strdup("BLUISH"), strdup("WILTED"), strdup("STAPLE"), strdup("SHIELD"), strdup("BOUNCE"), strdup("OCULAR"), strdup("RINGER"), strdup("MANAGE"), strdup("LITHIC"), strdup("BABIES"), strdup("CLOAKS"), strdup("HARLEY"), strdup("COMMON"), strdup("GIBLET"), strdup("SPONGE"), strdup("SAMPLE"), strdup("TRIFLE"), strdup("PELITE"), strdup("ALFRED"), strdup("COYOTE"), strdup("BISHOP"), strdup("ALLUDE"), strdup("FEMALE"), strdup("LOOSEN"), strdup("MARTHA"), strdup("ZAMBIA"), strdup("ANTHEM"), strdup("MASSES"), strdup("PETREL"), strdup("BORROW"), strdup("LEADER"), strdup("MARTON"), strdup("BARKIS"), strdup("DEPICT"), strdup("SLOVAK"), strdup("PLANET"), strdup("BROOKS"), strdup("LAPTOP"), strdup("EXHORT"), strdup("OXFORD"), strdup("IMPACT"), strdup("VACUUM"), strdup("ARCHAN"), strdup("NATURE"), strdup("TRYFAN"), strdup("AMEDEI"), strdup("TIGGER"), strdup("PALLID"), strdup("CITRIL"), strdup("SHODDY"), strdup("DUPLEX"), strdup("PROVES"), strdup("ROSTER"), strdup("WOOLER"), strdup("SODIUM"), strdup("NUPTSE"), strdup("TINGLY"), strdup("THIRTY"), strdup("BEETLE"), strdup("DURHAM"), strdup("BASALT"), strdup("BANKED"), strdup("UPTOWN"), strdup("THRILL"), strdup("WRETCH"), strdup("CURTSY"), strdup("EXETER"), strdup("BRAINY"), strdup("QUEENS"), strdup("QUIVER"), strdup("DEFTLY"), strdup("OBIWAN"), strdup("HANDED"), strdup("BUZZER"), strdup("FOYERS"), strdup("SUDOKU"), strdup("TOWHEE"), strdup("ENRAGE"), strdup("BREAST"), strdup("POWDER"), strdup("SCALBY"), strdup("COSINE"), strdup("WICKED"), strdup("HAGGIS"), strdup("AFLAME"), strdup("COFFEE"), strdup("UPWARD"), strdup("KODIAK"), strdup("PACIFY"), strdup("THROAT"), strdup("WELLOW"), strdup("FEEDER"), strdup("FLIGHT"), strdup("WILSON"), strdup("CHOPIN"), strdup("RESCUE"), strdup("YAWLER"), strdup("TIRADE"), strdup("GRAVES"), strdup("REFLEX"), strdup("PEEVED"), strdup("SOCCER"), strdup("WRYMUG"), strdup("CLIENT"), strdup("JOYOUS"), strdup("GOOGLE"), strdup("LAYING"), strdup("ELIDIR"), strdup("HANNAH"), strdup("LADDER"), strdup("APPEAL"), strdup("IMPEDE"), strdup("ALMOND"), strdup("CONNIE"), strdup("BUCKET"), strdup("ASCEND"), strdup("DICING"), strdup("MANUKA"), strdup("BUTTER"), strdup("CLUMSY"), strdup("ANKLES"), strdup("FOSSIL"), strdup("HINNEY"), strdup("HUNTER"), strdup("INFIRM"), strdup("YORKIE"), strdup("DIBABS"), strdup("CLINIC"), strdup("NITRIC"), strdup("VULCAN"), strdup("NARROW"), strdup("SARNIA"), strdup("STRAKE"), strdup("MUMBAI"), strdup("SYSTEM"), strdup("SHOWER"), strdup("LAUNCH"), strdup("SEARCH"), strdup("SKIING"), strdup("TISSUE"), strdup("CARTON"), strdup("OGMORE"), strdup("BELONG"), strdup("UTMOST"), strdup("ISLAND"), strdup("WINKLE"), strdup("SMOKED"), strdup("YONDER"), strdup("OBJECT")};
	if (debug)  {
			std :: cout << "Loading words..." << std :: endl;
	std :: cout << "Words loaded." << std :: endl;
	}
	final_word = wordList[day];

}




bool init()
{

	if (SDL_Init(SDL_INIT_VIDEO) > 0)
	    if (debug)
			std::cout << "HEY.. SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
	if (!(IMG_Init(IMG_INIT_PNG)))
		if (debug)
			std::cout << "IMG_init has failed. Error: " << SDL_GetError() << std::endl;
	if (TTF_Init()==-1)
		if (debug) 
			std::cout << "TTF_init has failed. Error: " << SDL_GetError() << std::endl;


	std :: cout << "Successfully initialized SDL!" << std::endl;
	importData();
	getWord();
	window.setIcon("/usr/local/Hexordle/assets/gfx/icon.png");
	return true;
}

bool SDL_init = init();





bool inWord(const char* wo,char letter) {
	for (int i = 0; i < 6; i++) {
		if (wo[i] == letter) {
			return true;
		}
	}
	return false;
}
int on_char = 1;
char* _1 = strdup("\0");
char* _2 = strdup("\0");
char* _3 = strdup("\0");
char* _4 = strdup("\0");
char* _5 = strdup("\0");
char* _6 = strdup("\0");
int char_forward = 0;




void resetGuess();


SDL_Event event;


/* yellow 
SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
*/

/* green
SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
*/

bool not_enough_chars = false;


SDL_Color white = { 255, 255, 255 };
SDL_Color black = { 0, 0, 0 };
SDL_Color green = { 97, 139, 85};
TTF_Font* font32 = TTF_OpenFont("/usr/local/Hexordle/assets/font/font.ttf", 32);
TTF_Font* font48 = TTF_OpenFont("/usr/local/Hexordle/assets/font/font.ttf", 48);
TTF_Font* font24 = TTF_OpenFont("/usr/local/Hexordle/assets/font/font.ttf", 24);
TTF_Font* font100 = TTF_OpenFont("/usr/local/Hexordle/assets/font/font.ttf", 100);
TTF_Font* font25 = TTF_OpenFont("/usr/local/Hexordle/assets/font/font.ttf", 25);
TTF_Font* font17 = TTF_OpenFont("/usr/local/Hexordle/assets/font/font.ttf", 17);
SDL_Texture* click2start = window.loadTexture("/usr/local/Hexordle/assets/gfx/click2start.png");
SDL_Texture* splashBgTexture = window.loadTexture("/usr/local/Hexordle/assets/gfx/splashbg.png");
SDL_Texture* bgTexture = window.loadTexture("/usr/local/Hexordle/assets/gfx/bg.png");
SDL_Texture* gameBgTexture = window.loadTexture("/usr/local/Hexordle/assets/gfx/gameBg.png");

int state = 0; //0 = title screen, 1 = game
bool gameRunning = true;
Uint64 currentTick = SDL_GetPerformanceCounter();
Uint64 lastTick = 0;
double deltaTime = 0;





void checkKeyPress () {
			const Uint8* keys = SDL_GetKeyboardState(NULL);
			if (!played) {
				played = true;
			}
			if (keys[SDL_SCANCODE_BACKSPACE]) {

				if (on_char == 2) {
					strcpy(_1, "\0");
					char_forward = 1;
				}
				else if (on_char == 3) {
					strcpy(_2, "\0");
					char_forward = 1;
				}
				else if (on_char == 4) {
					strcpy(_3, "\0");
					char_forward = 1;
				}
				else if (on_char == 5) {
					strcpy(_4, "\0");
					char_forward = 1;
				}
				else if (on_char == 6) {
					strcpy(_5, "\0");
					char_forward = 1;
				}
				else if (on_char == 7) {
					strcpy(_6, "\0");
					char_forward = 1;
				}
			}
			else if (keys[SDL_SCANCODE_A])
			{
				if (on_char == 1)
				{
					strcpy(_1, "A");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "A");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "A");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "A");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "A");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "A");
					char_forward = 2;
				}
			}


			else if (keys[SDL_SCANCODE_B])
			{
				if (on_char == 1)
				{
					strcpy(_1, "B");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "B");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "B");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "B");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "B");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "B");
					char_forward = 2;
				}
			}


			else if (keys[SDL_SCANCODE_C])
			{
				if (on_char == 1)
				{
					strcpy(_1, "C");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "C");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "C");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "C");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "C");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "C");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_D])
			{
				if (on_char == 1)
				{
					strcpy(_1, "D");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "D");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "D");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "D");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "D");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "D");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_E])
			{
				if (on_char == 1)
				{
					strcpy(_1, "E");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "E");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "E");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "E");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "E");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "E");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_F])
			{
				if (on_char == 1)
				{
					strcpy(_1, "F");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "F");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "F");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "F");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "F");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "F");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_G])
			{
				if (on_char == 1)
				{
					strcpy(_1, "G");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "G");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "G");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "G");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "G");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "G");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_H])
			{
				if (on_char == 1)
				{
					strcpy(_1, "H");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "H");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "H");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "H");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "H");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "H");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_I])
			{
				if (on_char == 1)
				{
					strcpy(_1, "I");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "I");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "I");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "I");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "I");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "I");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_J])
			{
				if (on_char == 1)
				{
					strcpy(_1, "J");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "J");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "J");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "J");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "J");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "J");
					char_forward = 2;
				}
			}





			else if (keys[SDL_SCANCODE_K])
			{
				if (on_char == 1)
				{
					strcpy(_1, "K");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "K");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "K");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "K");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "K");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "K");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_L])
			{
				if (on_char == 1)
				{
					strcpy(_1, "L");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "L");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "L");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "L");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "L");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "L");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_M])
			{
				if (on_char == 1)
				{
					strcpy(_1, "M");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "M");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "M");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "M");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "M");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "M");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_N])
			{
				if (on_char == 1)
				{
					strcpy(_1, "N");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "N");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "N");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "N");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "N");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "N");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_O])
			{
				if (on_char == 1)
				{
					strcpy(_1, "O");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "O");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "O");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "O");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "O");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "O");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_P])
			{
				if (on_char == 1)
				{
					strcpy(_1, "P");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "P");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "P");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "P");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "P");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "P");
					char_forward = 2;
				}
			}




			else if (keys[SDL_SCANCODE_Q])
			{
				if (on_char == 1)
				{
					strcpy(_1, "Q");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "Q");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "Q");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "Q");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "Q");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "Q");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_R])
			{
				if (on_char == 1)
				{
					strcpy(_1, "R");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "R");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "R");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "R");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "R");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "R");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_S])
			{
				if (on_char == 1)
				{
					strcpy(_1, "S");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "S");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "S");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "S");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "S");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "S");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_T])
			{
				if (on_char == 1)
				{
					strcpy(_1, "T");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "T");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "T");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "T");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "T");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "T");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_U])
			{
				if (on_char == 1)
				{
					strcpy(_1, "U");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "U");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "U");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "U");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "U");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "U");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_V])
			{
				if (on_char == 1)
				{
					strcpy(_1, "V");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "V");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "V");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "V");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "V");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "V");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_W])
			{
				if (on_char == 1)
				{
					strcpy(_1, "W");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "W");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "W");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "W");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "W");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "W");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_X])
			{
				if (on_char == 1)
				{
					strcpy(_1, "X");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "X");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "X");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "X");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "X");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "X");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_Y])
			{
				if (on_char == 1)
				{
					strcpy(_1, "Y");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "Y");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "Y");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "Y");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "Y");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "Y");
					char_forward = 2;
				}
			}



			else if (keys[SDL_SCANCODE_Z])
			{
				if (on_char == 1)
				{
					strcpy(_1, "Z");
					char_forward = 2;
				}
				else if (on_char == 2) {
					strcpy(_2, "Z");
					char_forward = 2;
				}
				else if (on_char == 3) {
					strcpy(_3, "Z");
					char_forward = 2;
				}
				else if (on_char == 4) {
					strcpy(_4, "Z");
					char_forward = 2;
				}
				else if (on_char == 5) {
					strcpy(_5, "Z");
					char_forward = 2;
				}
				else if (on_char == 6) {
					strcpy(_6, "Z");
					char_forward = 2;
				}

			}


			else if (keys[SDL_SCANCODE_RETURN]) {
				if (on_char == 7) {
					char* word = strdup("");
					strcat(word,_1);
					strcat(word,_2);
					strcat(word,_3);
					strcat(word,_4);
					strcat(word,_5);
					strcat(word,_6);
					std :: string tem = final_word;
					if (guess == 1) {
					guesses[0] = word;
					for (int i = 0; i < 6; i++) {

						if (word[i] == final_word[i]) {
							tem[i] = '\0';
							guess_acc[0][i] = 2;
						}
					}
					for (int i = 0; i <6; i++) {
						char letter = guesses[0][i];
						// check if a character is in a char*
						if (strchr(word, letter)) {
							// if it is, set the corresponding character in the word to the letter
							if (inWord(tem.c_str(),letter)) {
								// remove the letter from the tem variable
								tem[getPosition(tem.c_str(),letter)] = '\0';
								guess_acc[0][i] = 1;
							}
							else if (!inWord(tem.c_str(),letter) and guess_acc[0][i] != 2) {
								guess_acc[0][i] = 0;
							}
						}
						
					}
					}
					else if (guess == 2) {
					guesses[1] = word;
					for (int i = 0; i < 6; i++) {

						if (word[i] == final_word[i]) {
							tem[i] = '\0';
							guess_acc[1][i] = 2;
						}
					}
					for (int i = 0; i <6; i++) {
						char letter = guesses[1][i];
						// check if a character is in a char*
						if (strchr(word, letter)) {
							// if it is, set the corresponding character in the word to the letter
							if (inWord(tem.c_str(),letter)) {
								// remove the letter from the tem variable
								tem[getPosition(tem.c_str(),letter)] = '\0';
								guess_acc[1][i] = 1;
							}
							else if (!inWord(tem.c_str(),letter) and guess_acc[1][i] != 2) {
								guess_acc[1][i] = 0;
							}
						}
						
					}
					}
					else if (guess == 3) {
					guesses[2] = word;
					for (int i = 0; i < 6; i++) {

						if (word[i] == final_word[i]) {
							tem[i] = '\0';
							guess_acc[2][i] = 2;
						}
					}
					for (int i = 0; i <6; i++) {
						char letter = guesses[2][i];
						// check if a character is in a char*
						if (strchr(word, letter)) {
							// if it is, set the corresponding character in the word to the letter
							if (inWord(tem.c_str(),letter)) {
								// remove the letter from the tem variable
								tem[getPosition(tem.c_str(),letter)] = '\0';
								guess_acc[2][i] = 1;
							}
							else if (!inWord(tem.c_str(),letter) and guess_acc[2][i] != 2) {
								guess_acc[2][i] = 0;
							}
						}
						
					}
					}
					else if (guess == 4) {
					guesses[3] = word;
					for (int i = 0; i < 6; i++) {

						if (word[i] == final_word[i]) {
							tem[i] = '\0';
							guess_acc[3][i] = 2;
						}
					}
					for (int i = 0; i <6; i++) {
						char letter = guesses[3][i];
						// check if a character is in a char*
						if (strchr(word, letter)) {
							// if it is, set the corresponding character in the word to the letter
							if (inWord(tem.c_str(),letter)) {
								// remove the letter from the tem variable
								tem[getPosition(tem.c_str(),letter)] = '\0';
								guess_acc[3][i] = 1;
							}
							else if (!inWord(tem.c_str(),letter) and guess_acc[3][i] != 2) {
								guess_acc[3][i] = 0;
							}
						}
						
					}
					}
					else if (guess == 5) {
					guesses[4] = word;
					for (int i = 0; i < 6; i++) {

						if (word[i] == final_word[i]) {
							tem[i] = '\0';
							guess_acc[4][i] = 2;
						}
					}
					for (int i = 0; i <6; i++) {
						char letter = guesses[4][i];
						// check if a character is in a char*
						if (strchr(word, letter)) {
							// if it is, set the corresponding character in the word to the letter
							if (inWord(tem.c_str(),letter)) {
								// remove the letter from the tem variable
								tem[getPosition(tem.c_str(),letter)] = '\0';
								guess_acc[4][i] = 1;
							}
							else if (!inWord(tem.c_str(),letter) and guess_acc[4][i] != 2) {
								guess_acc[4][i] = 0;
							}
						}
						
					}
					}
					else if (guess == 6) {
					guesses[5] = word;
					for (int i = 0; i < 6; i++) {

						if (word[i] == final_word[i]) {
							tem[i] = '\0';
							guess_acc[5][i] = 2;
						}
					}
					for (int i = 0; i <6; i++) {
						char letter = guesses[5][i];
						// check if a character is in a char*
						if (strchr(word, letter)) {
							// if it is, set the corresponding character in the word to the letter
							if (inWord(tem.c_str(),letter)) {
								// remove the letter from the tem variable
								tem[getPosition(tem.c_str(),letter)] = '\0';
								guess_acc[5][i] = 1;
							}
							else if (!inWord(tem.c_str(),letter) and guess_acc[5][i] != 2) {
								guess_acc[5][i] = 0;
							}
						}
						
					}
					}
					if (not_enough_chars) {
						not_enough_chars = false;
					}
					if (guess_acc[guess-1][0] ==2 and guess_acc[guess-1][1] ==2 and guess_acc[guess-1][2] ==2 and guess_acc[guess-1][3] ==2 and guess_acc[guess-1][4] ==2 and guess_acc[guess-1][5] ==2) {
						// copy logic
						streak ++;
						if (streak > max_streak) {
							max_streak = streak;
						}
						play_count ++;
						switch (guess) {
							case 1:
								guess_1++;
								break;
							case 2:
								guess_2 ++;
								break;
							case 3:
								guess_3 ++;
								break;
							case 4:
								guess_4 ++;
								break;
							case 5:
								guess_5 ++;
								break;
							case 6:	
								guess_6 ++;
								break;
						}
						per = calculatePercentage(play_count,guess_1+guess_2+guess_3+guess_4+guess_5+guess_6);
						win = true;
						stat_delay = true;

					}
					if (strcmp(word, final_word) != 0 and guess ==6) {
						// copy logic
						streak = 0;
						play_count ++;
						per = calculatePercentage(play_count,guess_1+guess_2+guess_3+guess_4+guess_5+guess_6);
						lose = true;
						stat_delay = true;
					}
					resetGuess();
					saveData();
				}
				else {
					not_enough_chars = true;
				}
			}

}


void renderGame () {
	window.clear();

		window.render(0, 0, gameBgTexture);
		window.renderCenter(-15, -330, game_name, font100, white);
		int x_pos = width/2 - 240;
		int y_pos = 210;
		SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
		// 2 means forward, 1 means back, 0 means no  underfined
		if (char_forward == 2) {
			on_char++;
			char_forward = 0;
		}
		else if (char_forward == 1) {
			on_char--;
			char_forward = 0;
		}


		if (win) {
			int real_guess = guess-1;
			if (real_guess == 1) {
				window.renderCenter(0,300, "Genius!", font48, white);
			}
			else if (real_guess == 2) {
				window.renderCenter(0,300, "Magnificient!", font48, white);
			}
			else if (real_guess == 3) {
				window.renderCenter(0,300, "Impressive!", font48, white);
			}
			else if (real_guess == 4) {
				window.renderCenter(0,300, "Splendid!", font48, white);
			}
			else if (real_guess == 5) {
				window.renderCenter(0,300, "Great!", font48, white);
			}
			else if (real_guess == 6) {
				window.renderCenter(0,300, "Phew!", font48, white);
			}

		}
		else if (lose) {
			window.renderCenter(0,300, final_word, font48, white);
		}
		else if (not_enough_chars) {
			window.renderCenter(0,300, "Not enough characters", font48, white);
		}

		//create the wordle board
		// i is the columns and j is the rows
		for (int i = 0; i<6; i++) {
			for (int j = 0; j<6; j++) {
				SDL_Rect rect = {x_pos,y_pos,50,50};
				SDL_Rect rect_1 = {x_pos-1,y_pos-1,52,52};
				SDL_Rect rect_2 = {x_pos-2,y_pos-2,54,54};
				SDL_Rect rect_3 = {x_pos-3,y_pos-3,56,56};
				SDL_Rect rect_4 = {x_pos-4,y_pos-4,58,58};
				SDL_Rect rect_5 = {x_pos-5,y_pos-5,60,60};
				SDL_Rect rect_6 = {x_pos-6,y_pos-6,62,62};
				SDL_Rect rects[7] = {rect, rect_1, rect_2, rect_3, rect_4, rect_5, rect_6};
				if (guess == 1) {
					SDL_RenderDrawRects(window.getRenderer(), rects, 7);
				}

				/*

				COLORS DRAWING

				*/

				if (guess == 7 and i == 5) {
					if (j == 0){
					if (guess_acc[i][0] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 1){
						if (guess_acc[i][1] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 2){
						if (guess_acc[i][2] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 3){
						if (guess_acc[i][3] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 4){
						if (guess_acc[i][4] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 5){
						if (guess_acc[i][5] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
				}


				if (guess >= 6 and i == 4) {
					if (j == 0){
					if (guess_acc[i][0] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 1){
						if (guess_acc[i][1] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 2){
						if (guess_acc[i][2] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 3){
						if (guess_acc[i][3] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 4){
						if (guess_acc[i][4] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 5){
						if (guess_acc[i][5] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
				}


				if (guess >= 5 and i == 3) {
					if (j == 0){
					if (guess_acc[i][0] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 1){
						if (guess_acc[i][1] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 2){
						if (guess_acc[i][2] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 3){
						if (guess_acc[i][3] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 4){
						if (guess_acc[i][4] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 5){
						if (guess_acc[i][5] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
				}
					else{
					if (guess == 1) {
						if (i >= 1) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 2) {
						if (i >= 2) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess ==3) {
						if (i >= 3) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 4) {
						if (i >= 4) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 5) {
						if (i >= 5) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 6) {
						if (i >= 6) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					}

				if (guess >= 4 and i == 2) {
					if (j == 0){
					if (guess_acc[i][0] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 1){
						if (guess_acc[i][1] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 2){
						if (guess_acc[i][2] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 3){
						if (guess_acc[i][3] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 4){
						if (guess_acc[i][4] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 5){
						if (guess_acc[i][5] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
				}
					else{
					if (guess == 1) {
						if (i >= 1) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 2) {
						if (i >= 2) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess ==3) {
						if (i >= 3) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 4) {
						if (i >= 4) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 5) {
						if (i >= 5) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 6) {
						if (i >= 6) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					}
			

				if (guess >= 3 and i == 1) {
					if (j == 0){
					if (guess_acc[i][0] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 1){
						if (guess_acc[i][1] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 2){
						if (guess_acc[i][2] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 3){
						if (guess_acc[i][3] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 4){
						if (guess_acc[i][4] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 5){
						if (guess_acc[i][5] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
				}
					else{
					if (guess == 1) {
						if (i >= 1) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 2) {
						if (i >= 2) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess ==3) {
						if (i >= 3) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 4) {
						if (i >= 4) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 5) {
						if (i >= 5) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 6) {
						if (i >= 6) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					}

				if (guess >= 2) {
					if (i == 0) {
					if (j == 0){
					if (guess_acc[i][0] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][0] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 1){
						if (guess_acc[i][1] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][1] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 2){
						if (guess_acc[i][2] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][2] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 3){
						if (guess_acc[i][3] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][3] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 4){
						if (guess_acc[i][4] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][4] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					else if (j == 5){
						if (guess_acc[i][5] == 2) {
					SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,255);
					SDL_RenderFillRect(window.getRenderer(), &rect_6);
					SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 1) {
						SDL_SetRenderDrawColor(window.getRenderer(), 178,159,76,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
						SDL_SetRenderDrawColor(window.getRenderer(), 58, 58, 60, 255);
					}
					else if (guess_acc[i][5] == 0) {
						SDL_SetRenderDrawColor(window.getRenderer(), 58,58,60,255);
						SDL_RenderFillRect(window.getRenderer(), &rect_6);
					}
					}
					}
					else{
					if (guess == 1) {
						if (i >= 0) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 2) {
						if (i >= 1) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess ==3) {
						if (i >= 2) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 4) {
						if (i >= 3) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 5) {
						if (i >= 4) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					else if (guess == 6) {
						if (i >= 5) {
							SDL_RenderDrawRects(window.getRenderer(), rects, 7);
						}
					}
					}
				}
				

				/*


				****************************************************************
				TEXT DRAWING
				****************************************************************


				*/



				if (guess == 1 and i == 0) {
					if (j == 0) {
					if (strcmp(_1,"\0") != 0) {
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _1, font32, white);
					}
					}
					else if (j == 1) {
					if (strcmp(_2,"\0") != 0) {
						window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _2, font32, white);
					}

					}
					else if (j == 2) {
						if (strcmp(_3,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _3, font32, white);
						}
					}
					else if (j == 3) {
						if (strcmp(_4,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _4, font32, white);
						}
					}
					else if (j == 4) {
						if (strcmp(_5,"\0") != 0) {
							window.renderText((float)x_pos+12, (float)y_pos+y_off, _5, font32, white);
						}
					}
					else if (j == 5) {
						if (strcmp(_6,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _6, font32, white);
						}
					}
				}
				else if (i == 0) {
					if (guess == 0 or guess == 1 or guess == 2 or guess == 3 or guess == 4 or guess == 5 or guess == 6 or guess == 7) {
					if (j == 0) {
					char* temp = new char(guesses[0][0]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 1) {
					char* temp = new char(guesses[0][1]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 2) {
					char* temp = new char(guesses[0][2]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 3) {
					char* temp = new char(guesses[0][3]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 4) {
					char* temp = new char(guesses[0][4]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 5) {
					char* temp = new char(guesses[0][5]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					}
				}

				if (guess == 2 and i == 1) {
					if (j == 0) {
					if (strcmp(_1,"\0") != 0) {
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _1, font32, white);
					}
					}
					else if (j == 1) {
					if (strcmp(_2,"\0") != 0) {
						window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _2, font32, white);
					}

					}
					else if (j == 2) {
						if (strcmp(_3,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _3, font32, white);
						}
					}
					else if (j == 3) {
						if (strcmp(_4,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _4, font32, white);
						}
					}
					else if (j == 4) {
						if (strcmp(_5,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _5, font32, white);
						}
					}
					else if (j == 5) {
						if (strcmp(_6,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _6, font32, white);
						}
					}
				}
				else if (i ==1) {
					if (guess == 3 or guess == 4 or guess == 5 or guess == 6 or guess == 7) {
					if (j == 0) {
					char* temp = new char(guesses[1][0]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 1) {
					char* temp = new char(guesses[1][1]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 2) {
					char* temp = new char(guesses[1][2]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 3) {
					char* temp = new char(guesses[1][3]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 4) {
					char* temp = new char(guesses[1][4]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 5) {
					char* temp = new char(guesses[1][5]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					}
				}	

				if (guess == 3 and i == 2) {
					if (j == 0) {
					if (strcmp(_1,"\0") != 0) {
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _1, font32, white);
					}
					}
					else if (j == 1) {
					if (strcmp(_2,"\0") != 0) {
						window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _2, font32, white);
					}

					}
					else if (j == 2) {
						if (strcmp(_3,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _3, font32, white);
						}
					}
					else if (j == 3) {
						if (strcmp(_4,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _4, font32, white);
						}
					}
					else if (j == 4) {
						if (strcmp(_5,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _5, font32, white);
						}
					}
					else if (j == 5) {
						if (strcmp(_6,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _6, font32, white);
						}
					}
				}
				else if (i ==2) {
					if (guess == 4 or guess == 5 or guess == 6 or guess == 7) {
					if (j == 0) {
					char* temp = new char(guesses[2][0]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 1) {
					char* temp = new char(guesses[2][1]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 2) {
					char* temp = new char(guesses[2][2]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 3) {
					char* temp = new char(guesses[2][3]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 4) {
					char* temp = new char(guesses[2][4]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 5) {
					char* temp = new char(guesses[2][5]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					}
				}


				if (guess == 4 and i == 3) {
					if (j == 0) {
					if (strcmp(_1,"\0") != 0) {
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _1, font32, white);
					}
					}
					else if (j == 1) {
					if (strcmp(_2,"\0") != 0) {
						window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _2, font32, white);
					}

					}
					else if (j == 2) {
						if (strcmp(_3,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _3, font32, white);
						}
					}
					else if (j == 3) {
						if (strcmp(_4,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _4, font32, white);
						}
					}
					else if (j == 4) {
						if (strcmp(_5,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _5, font32, white);
						}
					}
					else if (j == 5) {
						if (strcmp(_6,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _6, font32, white);
						}
					}
				}
				else if (i ==3) {
					if (guess == 5 or guess == 6 or guess == 7) {
					if (j == 0) {
					char* temp = new char(guesses[3][0]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 1) {
					char* temp = new char(guesses[3][1]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 2) {
					char* temp = new char(guesses[3][2]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 3) {
					char* temp = new char(guesses[3][3]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 4) {
					char* temp = new char(guesses[3][4]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 5) {
					char* temp = new char(guesses[3][5]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					}
				}



				if (guess == 5 and i == 4) {
					if (j == 0) {
					if (strcmp(_1,"\0") != 0) {
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _1, font32, white);
					}
					}
					else if (j == 1) {
					if (strcmp(_2,"\0") != 0) {
						window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _2, font32, white);
					}

					}
					else if (j == 2) {
						if (strcmp(_3,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _3, font32, white);
						}
					}
					else if (j == 3) {
						if (strcmp(_4,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _4, font32, white);
						}
					}
					else if (j == 4) {
						if (strcmp(_5,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _5, font32, white);
						}
					}
					else if (j == 5) {
						if (strcmp(_6,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _6, font32, white);
						}
					}
				}
				else if (i ==4) {
					if (guess == 6 or guess == 7) {
					if (j == 0) {
					char* temp = new char(guesses[4][0]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 1) {
					char* temp = new char(guesses[4][1]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 2) {
					char* temp = new char(guesses[4][2]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 3) {
					char* temp = new char(guesses[4][3]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 4) {
					char* temp = new char(guesses[4][4]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 5) {
					char* temp = new char(guesses[4][5]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					}
				}





				if (guess == 6 and i == 5) {
					if (j == 0) {
					if (strcmp(_1,"\0") != 0) {
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _1, font32, white);
					}
					}
					else if (j == 1) {
					if (strcmp(_2,"\0") != 0) {
						window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _2, font32, white);
					}

					}
					else if (j == 2) {
						if (strcmp(_3,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _3, font32, white);
						}
					}
					else if (j == 3) {
						if (strcmp(_4,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _4, font32, white);
						}
					}
					else if (j == 4) {
						if (strcmp(_5,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _5, font32, white);
						}
					}
					else if (j == 5) {
						if (strcmp(_6,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _6, font32, white);
						}
					}
				}
				else if (i ==5) {
					if (guess == 6 or guess ==7) {
					if (j == 0) {
					char* temp = new char(guesses[5][0]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 1) {
					char* temp = new char(guesses[5][1]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 2) {
					char* temp = new char(guesses[5][2]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 3) {
					char* temp = new char(guesses[5][3]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 4) {
					char* temp = new char(guesses[5][4]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 5) {
					char* temp = new char(guesses[5][5]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					}
				}




				if (guess == 7 and i == 5) {
					if (j == 0) {
					if (strcmp(_1,"\0") != 0) {
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _1, font32, white);
					}
					}
					else if (j == 1) {
					if (strcmp(_2,"\0") != 0) {
						window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _2, font32, white);
					}

					}
					else if (j == 2) {
						if (strcmp(_3,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _3, font32, white);
						}
					}
					else if (j == 3) {
						if (strcmp(_4,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _4, font32, white);
						}
					}
					else if (j == 4) {
						if (strcmp(_5,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _5, font32, white);
						}
					}
					else if (j == 5) {
						if (strcmp(_6,"\0") != 0) {
							window.renderText((float)x_pos+x_off, (float)y_pos+y_off, _6, font32, white);
						}
					}
				}
				else if (i ==6) {
					if (guess ==7) {
					if (j == 0) {
					char* temp = new char(guesses[6][0]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 1) {
					char* temp = new char(guesses[6][1]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 2) {
					char* temp = new char(guesses[6][2]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 3) {
					char* temp = new char(guesses[6][3]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 4) {
					char* temp = new char(guesses[6][4]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					else if (j == 5) {
					char* temp = new char(guesses[6][5]);
					window.renderText((float)x_pos+x_off, (float)y_pos+y_off, temp, font32, white);
					}
					}
				}
				// go to the next box
				x_pos += 80;
			}
			// reset the x and go to the next line
			x_pos = width/2-240;
			y_pos += 80;
		}
		int _x_pos, _y_pos;
		SDL_GetMouseState(&_x_pos, &_y_pos);
		SDL_Rect stats_button = {width/2+300, height/2-160, 200, 70};
		SDL_SetRenderDrawBlendMode(window.getRenderer(), SDL_BLENDMODE_BLEND);
		if (_x_pos >= width/2+300 and _x_pos <= width/2+500 and _y_pos >= 240 and _y_pos <= 310 and stats_screen == false) {
			SDL_SetRenderDrawColor(window.getRenderer(), 89, 128, 78, 220);
		}
		else {
			SDL_SetRenderDrawColor(window.getRenderer(), 97,139,85,220);
		}
		SDL_RenderFillRect(window.getRenderer(), &stats_button);
		SDL_SetRenderDrawBlendMode(window.getRenderer(), SDL_BLENDMODE_NONE);
		window.renderCenter(400, -125, "Stats", font32, white);
		if (stats_screen) {
			SDL_Rect end_screen = {width/2-300, height/2-270, 600, 547};
			SDL_Rect end_screen_back = {0, 0, width, height};
			SDL_SetRenderDrawColor(window.getRenderer(),9,9,9,120);
			SDL_SetRenderDrawBlendMode(window.getRenderer(), SDL_BLENDMODE_BLEND);
			SDL_RenderFillRect(window.getRenderer(), &end_screen_back);
			SDL_SetRenderDrawColor(window.getRenderer(),18,18,19,255);
			SDL_SetRenderDrawBlendMode(window.getRenderer(), SDL_BLENDMODE_NONE);
			SDL_RenderFillRect(window.getRenderer(), &end_screen);
			window.renderCenter(0,-180,"STATISTICS",font48,white);
			window.renderCenter(0,-20,"GUESS DISTRIBUTION",font17,white);
			window.renderCenter(-120,-120,std::to_string(play_count).c_str(),font32,white);
			window.renderCenter(-120,-100,"Played",font17,white);

			window.renderCenter(-40,-120,std::to_string(per).c_str(),font32,white);
			window.renderCenter(-35,-100,"Win %",font17,white);
			
			window.renderCenter(40,-120,std::to_string(streak).c_str(),font32,white);
			window.renderCenter(40,-100,"Current",font17,white);
			window.renderCenter(40,-80,"Streak",font17,white);

			window.renderCenter(120,-120,std::to_string(max_streak).c_str(),font32,white);
			window.renderCenter(120,-100,"Max",font17,white);
			window.renderCenter(120,-80,"Streak",font17,white);
			std :: string guess_1_text = "1: ";
			guess_1_text += std::to_string(guess_1);
			if (guess == 2 && win) {			window.renderCenter(-180,20,guess_1_text.c_str(),font24,green);}
			else {window.renderCenter(-180,20,guess_1_text.c_str(),font24,white);}
			std :: string guess_2_text = "2: ";
			guess_2_text += std::to_string(guess_2);
			if (guess == 3 && win) {window.renderCenter(-180,60,guess_2_text.c_str(),font24,green);}
			else {window.renderCenter(-180,60,guess_2_text.c_str(),font24,white);}
			std :: string guess_3_text = "3: ";
			guess_3_text += std::to_string(guess_3);
			if (guess == 4 && win) {			window.renderCenter(-180,100,guess_3_text.c_str(),font24,green);}
			else {window.renderCenter(-180,100,guess_3_text.c_str(),font24,white);}
			std :: string guess_4_text = "4: ";
			guess_4_text += std::to_string(guess_4);
			if (guess == 5 && win) {			window.renderCenter(-180,140,guess_4_text.c_str(),font24,green);}
			else {window.renderCenter(-180,140,guess_4_text.c_str(),font24,white);}
			std :: string guess_5_text = "5: ";
			guess_5_text += std::to_string(guess_5);
			if (guess == 6 && win) {			window.renderCenter(-180,180,guess_5_text.c_str(),font24,green);}
			else {window.renderCenter(-180,180,guess_5_text.c_str(),font24,white);}
			std :: string guess_6_text = "6: ";
			guess_6_text += std::to_string(guess_6);
			if (guess == 7 && win) {			window.renderCenter(-180,220,guess_6_text.c_str(),font24,green);}
			else {window.renderCenter(-180,220,guess_6_text.c_str(),font24,white);}
			SDL_Rect share = {width/2+60, height/2+160, 200, 70};
			int x_pos, y_pos;
			SDL_GetMouseState(&x_pos, &y_pos);
			if (win or lose) {
				if (x_pos >= width/2+60 and x_pos <= width/2+260 and y_pos >= height/2+160 and y_pos <= height/2+230) {
					SDL_SetRenderDrawColor (window.getRenderer(), 89, 128, 78, 255);
					SDL_RenderFillRect(window.getRenderer(), &share);
					window.renderCenter(160,195,"SHARE",font32,white);
				}
				else {
				SDL_SetRenderDrawColor (window.getRenderer(), 97, 139, 85, 255);
				SDL_RenderFillRect(window.getRenderer(), &share);
				window.renderCenter(160,195,"SHARE",font32,white);
				}
			if (copy_button_pressed) {

					SDL_Rect back = {width/2+60, height/2+160, 200, 10};
					SDL_Rect share = {width/2+60, height/2+170, 200, 70};
					SDL_SetRenderDrawColor (window.getRenderer(), 89, 128, 78, 255);
					SDL_RenderFillRect(window.getRenderer(), &share);
					SDL_SetRenderDrawColor (window.getRenderer(), 18,18,19, 255);
					SDL_RenderFillRect(window.getRenderer(), &back);
					window.renderCenter(160,205,"SHARE",font32,white);
					copy_button_pressed = false;
			}
			}


		}

		window.display();
}



void resetGuess () {
	guess++;
	on_char = 1; 
	strcpy(_1, "\0");
	strcpy(_2, "\0");
	strcpy(_3, "\0");
	strcpy(_4, "\0");
	strcpy(_5, "\0");
	strcpy(_6, "\0");


}

void titleScreen()
{
    // controls the splash screen wait time (increase 2000 to make it longer) divides by 1k
	if (SDL_GetTicks() < 2000)
	{
		//Get our controls and events
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				gameRunning = false;
				break;
			}
		}

		window.clear();

		window.render(0, 0, bgTexture);
		// splash background
		window.render(width/2-320, height/2-240, splashBgTexture);
		window.renderCenter(0, 0 + 3, "Akshar Desai", font25, black);
		window.renderCenter(0, 0, "Akshar Desai", font25, white);
		window.display();
	}
	else
	{
		lastTick = currentTick;
		currentTick = SDL_GetPerformanceCounter();
		deltaTime = (double)((currentTick - lastTick)*1000 / (double)SDL_GetPerformanceFrequency() );

		//Get our controls and events
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				gameRunning = false;
				break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    gameRunning = false;
                }
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					state = 1;
				}
				break;
            }
		}
		window.clear();
		window.render(0, 0, bgTexture);
        double temp = SDL_sin(SDL_GetTicks()*(8.14/1500));
		window.renderCenter(0, 40 - 100 - 45 + 4*temp, game_name, font100, black);
        window.renderCenter(0, 40 - 100 - 50+4*temp, game_name, font100, white);
		// click background
		window.render(width/2-318, height/2-240, click2start);
		window.renderCenter(4, 240 - 48 + 3 - 16*5, "CLICK TO PLAY", font32, black);
		window.renderCenter(4, 240 - 48 - 16*5, "CLICK TO PLAY", font32, white);
		window.display();
	}
}


void gameScreen()
{
		//Get our controls and events
		while (SDL_PollEvent(&event))
		{
					if (!win and !lose) {
		checkKeyPress();
					}
			switch(event.type)
			{
			case SDL_QUIT:
				gameRunning = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				int x_pos, y_pos;
				SDL_GetMouseState(&x_pos, &y_pos);
				// TODO: fix numbers
				if (x_pos >= width/2+300 and x_pos <= width/2+500 and y_pos >= 240 and y_pos <= 310 and stats_screen == false) {				
						stats_screen = true;
				}
				else if (x_pos >= width/2+60 and x_pos <= width/2+260 and y_pos >= height/2+160 and y_pos <= height/2+230 and stats_screen and (win or lose)) {
					SDL_SetClipboardText(create_output(guess_acc,day,guess-1,lose).c_str());
					copy_button_pressed = true;
				}
				else if ((x_pos <= width/2-300 or x_pos >= width/2+300) and stats_screen) {
					stats_screen = false;
				}

			}
		}

		renderGame();
		if (stat_delay) {
			SDL_Delay(1400);
			stat_delay = false;
			stats_screen = true;
		}
		else {
		SDL_Delay(20);
		}
}

void game()
{
	if (state == 0) /* title screen */ {
    	titleScreen();
	}
	else if (state == 1) /* game screen */ {
		gameScreen();
	}
}



int main(int argc, char* argv[])
{
	while (gameRunning)
	{
		game();
	}

	window.cleanUp();
	TTF_CloseFont(font32);
	TTF_CloseFont(font24);
    TTF_CloseFont(font48);
	TTF_CloseFont(font100);
	TTF_CloseFont(font25);
	TTF_CloseFont(font17);
	SDL_Quit();
	TTF_Quit();
	return 0;
}
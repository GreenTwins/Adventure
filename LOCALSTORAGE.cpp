#include "LOCALSTORAGE.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>


savedData& savedData::getInstance() {
	static savedData instance;
	return instance;
}

savedData::savedData() {
	std::vector<std::string>savedInfo;
	std::string playerfolderPath = "C:\\Users\\aaron\\source\\CPP\\adventure\\Character\\AdventureGame\\PlayerData";
	savedInfo.push_back(playerfolderPath);
	std::string enemyfolderPath = "C:\\Users\\aaron\\source\\CPP\\adventure\\Character\\AdventureGame\\EnemyData";
	savedInfo.push_back(enemyfolderPath);
	std::string inventoryfolderPath = "C:\\Users\\aaron\\source\\CPP\\adventure\\Character\\AdventureGame\\Inventory";
	savedInfo.push_back(inventoryfolderPath);

	for (auto& data : savedInfo) {
		try {
			std::filesystem::create_directory(data);
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cerr << "Error creating folder " << e.what() << std::endl;
		}
	}
	std::cout << "files created" << std::endl;
}
void savedData::uploadData(const Player& p) {
	std::ofstream filename("save_character_file.txt", std::ios::out);

	std::cout << "saving data..." << std::endl;
	filename << p.location << std::endl;
	filename << p.getName() << std::endl;
	filename << p.getHP() << std::endl;
	filename << p.getMP() << std::endl;
	filename << p.getStr() << std::endl;
	filename << p.getDef() << std::endl;
	filename << p.getSpd() << std::endl;
	filename << p.getDodge() << std::endl;
	filename << p.attack1.first << std::endl;
	filename << p.attack1.second << std::endl;
	filename << p.attack2.first << std::endl;
	filename << p.attack2.second << std::endl;
	filename << p.attack3.first << std::endl;
	filename << p.attack3.second << std::endl;
	filename << p.attack4.first << std::endl;
	filename << p.attack4.second << std::endl;
	filename << p.attack5.first << std::endl;
	filename << p.attack5.second << std::endl;
	filename << p.attack6.first << std::endl;
	filename << p.attack6.second << std::endl;

	filename.close();

	std::cout << "Data saved locally" << std::endl;
}

void savedData::loadData() {
	std::string playerPath = "C:\\Users\\aaron\\AdventureGame\\PlayerData";
	std::string enemyPath;
	std::string invPath;
	std::vector<std::filesystem::path>folderData;
	int input{ 0 };
	//will use threads for this process of gathering, displaying and executing
	std::cout << " Choose which Character you want to load" << std::endl;
		try {
			for (const auto& data : std::filesystem::directory_iterator(playerPath)) {
				int i = 1;
				if (std::filesystem::is_regular_file(data)) {
					folderData.push_back(data.path().filename()); 
					std::cout<<i<<".) " << data.path().filename() << std::endl;
				}
			}
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cerr << " Error accessing folder: " << e.what() << std::endl;
			return;
		}
		std::cin >> input;
		/*switch (input) {
		case 1: 
			grabfromLoadLocal(folderData[0]);
			break;
		}*/
	
}
savedData::~savedData() {}
//void savedData::grabfromLoadLocal(const std::filesystem::path& a) {
//	std::cout << "hi";
//}




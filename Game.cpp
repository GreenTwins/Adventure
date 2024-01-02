#include "Game.h"
#include <iostream>
#include "SQLCONN.h"
#include "LOCALSTORAGE.h"
#include <random>
#include <string>
#include <memory>

/******************************************************************************************************
GAME CLASS init,cleaner, getters and setters

*******************************************************************************************************/

Game::Game() {
	/*MainMenu& master = MainMenu::getInstance();
	master.display();*/
	enemyList.clear();
	locations.clear();
	//BossReq.push_back("Troll"); not used
	uploadWorldMap();
}
Game::~Game() {
	std::cout << "Game instance deleted" << std::endl;
}
void Game::fromSQL(bool i) {
	onserver = i;
}
void Game::fromLocal(bool i) {
	onlocal = i;
}
void Game::uploadWorldMap() {
	std::vector<std::string>world_mapNames = { "Realm of Qiteoria", "Realm of Reperion: Dark Forest","Ruined Settlement of Ibburyon","The Echo Lands","The Barrens","The Reach of Dreams","Lands of the Broken","The Hells Vale" };
	
	for (std::string maps : world_mapNames) {
		std::map<std::string, bool>mapItem;
		 mapItem.insert(std::make_pair(maps, false));
		 world_map.push_back(mapItem);
	}
	
	

}

Game& Game::getinstance() {
	static Game instance;
	return instance;
}


bool Game::isSQL()const {
	return onserver;
}
bool Game::isLocal()const {
	return onlocal;
}
void Game::getLocationName(int loc) {
	std::map<std::string, bool>currentLocation = world_map[loc - 1];
	for (auto& mapItem : currentLocation) {
		std::cout << "You've entered: " << mapItem.first << "\n";
	}
}
int Game::TravelonWorldMap() {
	int location_num = 1;
	for (auto& map : world_map) {
		for (auto& elements : map) {
			//elements.second
			if (playerN.getLvl()>=location_num) {
				std::cout << "Location " << location_num << ": " << elements.first << '\n';

			}
			else {
				std::cout << "Location " << location_num << ": Unavailable at your current level" << std::endl;
			}
			location_num++;
		}
	}
	int travelLocation = 0;
	std::cout << "\n Which locaiton do you want to travel to?: "; 
	std::cin >> travelLocation;

	if (travelLocation > playerN.getLvl()) {//player level and the boolean should match for access
		std::cout << "This location isnt available to you \n";
		return -1;
	}
	else if (travelLocation > world_map.size()) {
		std::cout << "The choice given is invalid \n";
		return -1;
	}
	std::cout << "Travelling to new region....\n";
    //get the name
	/*for (auto location : world_map) {
		for (auto location_name : location) {
			std::cout << "You've arrived at: " << location_name.first << "\n";
		}
	}*/
	return travelLocation;
}
void Game::loadEnemies(int loc, int dunNum, std::vector<Enemy>&e) {
	try {
		SQLCONN &enemyGrab=SQLCONN::createInstance();
		enemyGrab.getEnemies(loc, dunNum, e);
		//enemyGrab.disconnect();
		}
	catch (bool result) {
		std::cout << "Database failed to connect" << std::endl;
	}
}


/******************************************************************************************************
GAME CLASS player association

*******************************************************************************************************/
void Game::createPlayer(std::string n) {
	playerN=Player::Player(n);
	newChar = true;
	playerN.init();
}

void Game::displayInventory() {
	char response;
	int choice;
	std::cout << "Do you want to equip an item?(Y/N): ";
	std::cin >> response;
	std::cout << "\n";
	if (response == 'y' || response == 'Y') {
		for (int i = 0; i < Inventory.size(); i++) {
			Item item = Inventory[i];
			std::cout << i + 1 << ".)Name: " << item.getItemName() << "\t Type: " << item.getType() << "\t Equpped: " <<std::boolalpha<<item.getIsEquip()<<std::endl;
		}
		std::cout << "Which item do you want to equip? ";
		if (std::cin >> choice) {
			if (Inventory.size() < 1) {
				std::cout << "Your inventory is empty \n";
				return;
			}
			else if ((choice - 1) > Inventory.size()) {
				std::cout << "Your inventory is limited to " << Inventory.size() << " items\n";
				return;
			}
			equipItem(Inventory[choice - 1]);
		}
		else {
			std::cin.clear();
			std::string escRequest;
			std::cin >> escRequest;
			std::cout << escRequest << " isnt a valid command\n";
			std::cin.clear();
			return;
		}
	}
	else {
		std::cout << "		" << playerN.getName() << "'s Inventory" << std::endl;
		std::cout << "ITEM NAME--------------ITEM STATS------------EQUIPPED----" << std::endl;
		int counter = 1;
		for (auto& item : Inventory) {
			std::cout << counter<<".) "<<item.getItemName() << "\t" << item.getStats().first << ": +" << item.getStats().second << "\t Equipped: " << std::boolalpha << item.getIsEquip() << std::endl;
			counter++;
		}
	}
}
void Game::unequipItem(Item& it) {
	std::string removeItem = it.getItemName();
	for (auto& gotIt : Inventory) {
		if (gotIt.getItemName() == removeItem) {
			gotIt.set_EquipOff(); //turns inventory item off
			activeItems[gotIt.getItemName()] = false; //turns equip marker off
			int buffRemoved = gotIt.getStats().second;
			if (gotIt.getStats().first == "Def") {
				playerN.setDef(playerN.getDef() - buffRemoved);
			}
			else if(gotIt.getStats().first =="Str"){
				playerN.setStr(playerN.getStr() - buffRemoved);
			}
			else {
				std::cout << "Empty potion removed" << std::endl;
			}
			activeItems[gotIt.getType()] = false;
		}
	}
}

void Game::equipItem(Item& it) {
	char choice;
	std::string itemType = it.getType();
	if (activeItems[itemType] == false) {
		activeItems[itemType]=true;
		it.set_EquipON();
		int buffAmt = it.getStats().second;
		if (it.getStats().first == "HP") {
			if (playerN.getHP() != playerN.getMaxHP()) {
				if ((playerN.getHP() + it.getStats().second) <= playerN.getMaxHP()) {
					//its not more than maxHP
					playerN.setHP(playerN.getHP() + buffAmt);
					//return;
				}
				else {
					//you waste some but fill HP
					playerN.setHP(playerN.getMaxHP());
				}
			}
			//make sure potion is removed after set ON
			it.set_EquipOff();
			activeItems[itemType] = false;
			remove_item(it);
			return;
		}
		if (it.getStats().first == "Def") {
			playerN.setDef(playerN.getDef() + buffAmt);
			return;
		}
		if (it.getStats().first == "Str") {
			playerN.setStr(playerN.getStr() + buffAmt);
		}
	}

 else {
	 // spot is occupied
	std::cout << "There seems to be an item equipped for this position. Do you wish to swap?(Y/N): ";
	std::cin >> choice;
	if (choice == 'y' || choice == 'Y') {
		// find the active item, turn it off and turn this one on
		for (auto& foundItem : Inventory) {
			if ((foundItem.getType() == itemType) && (foundItem.getIsEquip())) {
				foundItem.set_EquipOff();
				unequipItem(foundItem); // unequip previous item
				break; // stop the loop after finding and unequipping the item
			}
		}

		it.set_EquipON(); // equip the new item
		activeItems[itemType] = true;

		int buffAmt = it.getStats().second;
		if (it.getStats().first == "HP") {
			if (playerN.getHP() != playerN.getMaxHP()) {
				if ((playerN.getHP() + it.getStats().second) <= playerN.getMaxHP()) {
					//its not more than maxHP
					playerN.setHP(playerN.getHP() + buffAmt);
					//return;
				}
				else {
					//you waste some but fill HP
					playerN.setHP(playerN.getMaxHP());
				}
			}
		}
		else if (it.getStats().first == "Def") {
			playerN.setDef(playerN.getDef() + buffAmt);
		}
		else if (it.getStats().first == "Str") {
			playerN.setStr(playerN.getStr() + buffAmt);
		}
	}
	else {
		std::cout << "Thats not a valid choice\n";
	}
	}
	
}

void Game::remove_item(Item a) {
	auto it = std::find(Inventory.begin(), Inventory.end(), a);

	if (it != Inventory.end()) {
		Inventory.erase(it);
	}
}

/******************************************************************************************************
GAME CLASS game loading and instances

*******************************************************************************************************/

bool Game::loadGame() {
	int choice{ 0 };
	std::string playername;
	bool conn_success = false;
	std::cout << "Are you loading from a server or local storage?: ";
	std::cout << "1.) Server" << "\t" << "2.) Local Storage" << std::endl;
	std::cin >> choice;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');//clear anything remaining in the stream so we can get playername data
	switch (choice) {
	case 1: 
	{
		//check SQL server
		SQLCONN& connection = SQLCONN::createInstance();
		if (connection.connect()) {
			//gather avail chars in dB & display
			if (connection.displayNames()) {
				fromSQL(true);
				int choice;
				std::cout << "\n Which character would you like to load?:  ";
				std::cin >> choice;
				if (choice <= connection.playerList.size()) {
					std::cout << connection.playerList[choice - 1] << std::endl;
					playerN.changeName(connection.playerList[choice - 1]);
					conn_success = true;
				}
				else {
					std::cout << "You didnt choose a given number" << std::endl;
				}
				//let user choose char
				//loadPlayer();
				
			}
			else {
				std::cout << "There is no data to load" << std::endl;
			}
			
			connection.disconnect();
		}
		
	
		//load data into Player class
		
		break;
	}
	case 2: {
		//check local storage
		break;
	}
	default: 
		std::cout << "Please choose a correct option" << std::endl;
		break;
	}

	return conn_success;
}


bool Game::PrePlay() {
	GameInit = true;
	bool tryAgain = true;
	bool success = false;
	char option;
	currentDunLvl = 1;
	currentDunNum = 1;
	playerN.location = currentDunLvl;
	while (tryAgain) {
		getLocationName(1);//starting new
		Map newMap;
		std::cout << "creating paths" << std::endl;
		newMap.createPaths(currentDunLvl);
		Game::getinstance().loadEnemies(1, 1,Game::enemyList);
		std::cout << Game::getinstance().enemyList.size();
		if (play(newMap)) {
			//go back to island
			GameInit = false;
			success = true;
			tryAgain = false;
		}
		//got back to main menu or try again
		else {
			std::cout << "You have died. Would you like to try again? Type C to continue or Q to quit: ";
			std::cin >> option;
			if (option == 'Q' || option == 'q') {
				tryAgain = false;
			}
			Game::getinstance().playerN.refillHP();
		}
	}
	return success;
	}
	

bool Game::play(Map& currentMap) {
	system(CLEAR_SCREEN);
	currentMap.makeMove(1);
	
	if (playerN.getHP() > 0) {
		system(CLEAR_SCREEN);
		std::cout << "You've entered the Boss room" << std::endl;
		if (currentMap.bossBattle(currentDunLvl, currentDunNum, playerN)) {
			/*playerN.setGold(playerN.getcurrentMap.totalGold);
			playerN.setXP(currentMap.totalXP);*/
			return true;
		}
		return false;
	}
	std::cout << "You've died" << std::endl;
	return false;
}

void Game::loadAllMissions() {
	//this can be edited to add more
	std::vector<std::string>Desc;
	std::vector<std::string>Names;
	int currentLocation = Game::getinstance().playerN.location;
	AllMissions.clear();
	if (currentLocation == 1) {
		Desc= { "Grove", "Thicket", "Woodland", "backwoods", "growth", "woods", "woods", "wildwood", "woodlot", "park" ,"timber", "wilderness" };
		Names= { "Wandering ", "Treacherous ", "Mystic ", "Undergrowth ", "Bald ", "Foggy ", "Myswerks ", "Elevated ", "Hyde " };
	}
	else if(currentLocation ==2) {
		//will update later
		Names = { "Evergreen ", "Perfect ", "Untamed ", "Green ", "White ", "High ", "Suicide ", "Hanging Neck ", "Enders ", "Jaded " ,"North ", "Dark " };
		Desc = { "Grim ", "Wilderness ", "Bloodweed ", "Forest ", "Pines ", "Covert ", "Holt ", "Glade ","Wilds"};
	}
	else {
		Names = { "The Enders", "Forsaken", "The abyssal", "Ketes", "Lord Kens", "The silent", "", "The Nightmare", "Petrified", "Crumbling" ,"Silent" };
		Desc = { "Vault ", "Borough ", "Blight ", "abandoned city ", "Outpost ", "Covert ", "village ","Lands", "ruins", "wreckage"};
	}
	for (int i = 0; i < 6; ++i) {
		int mapDesc = rand() % (Desc.size()) + 1;
		int mapName = rand() % (Names.size()) + 1;
		std::string MissionTitle = ((Names[mapName-1]) + (Desc[mapDesc-1]));
		locationforMissions[i+1] = MissionTitle; //this gets pulled to give player and map the dungeon number

		std::map<int, std::string>MapData; 
		//locationforMissions[i] = MissionTitle;//load dunNum
		MapData[i+1] = MissionTitle; //this just gives the title and the actual location
		//location, dun num and name are required for the next 2 steps
		AllMissions.push_back(MapData);
	}
	
	
}
void Game::displayMapsAvailable() {
	//randomize pick from listofNames based on currentLocation via player
	loadAllMissions();
	std::map<int, std::string>ShownMissions;
	std::vector<std::map<int, std::string>>holder;
	std::map<int, std::string>found;
	int currentLocation = Game::getinstance().playerN.location;
	std::map<int, int>choiceHistory;
	int missionchoice = 0;
	int i = 1;

	while (holder.size() < 3) {
		//any duplicate titles can be found here. We arent making sure that doesnt happen. Should do for the loading of 6
		int choice = (rand() % (AllMissions.size()) + 1);
		if (choiceHistory[choice]==0) {
			choiceHistory[choice]++;
			found= AllMissions[choice - 1];
			holder.push_back(found);
			for (auto& item : found) {
				std::cout << i << ".) " << item.second << std::endl;
				++i;
			}
		}
	}
	
	
	std::cout << "Which mission do you choose?: ";
	std::cin >> missionchoice;
	if (missionchoice > 3) {
		std::cout << "That wasnt a valid option \n";
		return;
	}
	ShownMissions = holder[missionchoice - 1];
	for (auto& item : ShownMissions) {
		currentDunNum = item.first;
		ActiveMissionName = item.second;
	}
	
}

bool Game::startMission() {
	bool tryAgain = true;
	bool success = false;
	char option;
	
	currentDunLvl = playerN.location;//this will change and each location on the map will have a distinct number
	while (tryAgain) {
		//getLocationName(currentDunNum);//we can just pass the name in from prev function to get name of dungeon entered
		Map newMap;
		newMap.createPaths(currentDunLvl);
		std::cout << "You've entered: " << ActiveMissionName << std::endl;
		Game::getinstance().loadEnemies(currentDunLvl, currentDunNum, Game::enemyList);
		std::cout << Game::getinstance().enemyList.size();
		if (play(newMap)) {
			//go back to island
			success = true;
			tryAgain = false;
		}
		//got back to main menu or try again
		else {
			std::cout << "You have died. Would you like to try again? Type C to continue or Q to quit: ";
			std::cin >> option;
			if (option == 'Q' || option == 'q') {
				tryAgain = false;
			}
			Game::getinstance().playerN.refillHP();
			
		}
	}
	return success;
}
void Game::loadStore() {
	if ((StoreWeapons.size() < 1) && (StorePotions.size() < 1) && (StoreWearables.size() < 1)) {
		if (!SQLCONN::createInstance().grabStoreData(playerN.getLvl())) {
			return;
		}
		
	}
	std::cout << "Store loaded" << std::endl;
}
void Game::displayStore(int lvl) {
	/*std::cout << StoreWeapons.size() << std::endl;
	std::cout << StoreWearables.size() << std::endl;
	std::cout << StorePotions.size() << std::endl;*/
	/*STORE requires a minimum of 3 potions to appear and a combo of weapons or wearables*/
	std::vector<Item>StoreDisplay;
	srand(time(NULL));
	for (int i = 0; i < 4; ++i) {
		Item potion = StorePotions[rand() % (StorePotions.size()-1) + 1];
		StoreDisplay.push_back(potion);
	}
	for (int j = 0; j < 4; ++j) {
		int randompick = (rand() % 6 + 1); //
		if (randompick % 2 == 0) {//if even
			Item weapon = StoreWeapons[rand() % (StoreWeapons.size()-1) + 1];
			StoreDisplay.push_back(weapon);
		}
		else {
			Item wearable = StoreWearables[rand() % (StoreWearables.size()-1) + 1];
			StoreDisplay.push_back(wearable);
		}
	}

	//NOW DISPLAY THE STORE
	std::cout << "Current gold available: " << Game::getinstance().playerN.getGold() << std::endl;
	for (int i = 0; i < StoreDisplay.size(); ++i) {
		std::string name = StoreDisplay[i].getItemName();
		int price = StoreDisplay[i].getPrice();
		std::string type = StoreDisplay[i].getType();
		int buff = (StoreDisplay[i].getStats()).second;
		std::cout << i + 1 << ".) Name:  " << name << "\t Price: " << price << "\t Type: " << type << "\t Buff: " << buff << std::endl;
	}
	int choice;
	std::cout << "What would you like to purchase?[type 'none' to exit]: ";
	
	if (std::cin >> choice) {
		if (choice <= StoreDisplay.size()) {
			if (StoreDisplay[choice - 1].getPrice() <= playerN.getGold()) {
				int goldSpent = StoreDisplay[choice - 1].getPrice();
				std::cout << "Purchase successful. Go to inventory to use" << std::endl;
				Inventory.push_back(StoreDisplay[choice - 1]);
				playerN.setGold(playerN.getGold() - goldSpent);
			}
			else {
				std::cout << "You dont have enough money" << std::endl;
			}
			char response;
			std::cout << "Do you still want to shop?(Y/N): ";
			std::cin >> response;
			if (response == 'y' || response == 'Y') {
				return displayStore(lvl);
			}
			else {
				std::cout << "Come again!" << std::endl;
			}
		}
		else {
			std::cout << "That choice isnt a valid purchase option \n";
		}
	}
	else {
		std::cin.clear();
		std::string escReq;
		std::cin >> escReq;
		std::cout << "Leaving store... \n";
	}
}



//MAIN MENU

/******************************************************************************************************
MAIN MENU CLASS init,cleaner, getters and setters

*******************************************************************************************************/
MainMenu::MainMenu() {}
MainMenu::~MainMenu() {}

MainMenu& MainMenu::getInstance() {
	static MainMenu instance;
	return instance;
}


/******************************************************************************************************
MAIN MENU CLASS only visible function: display() shows the new game, load game and save game options

Each option chosen will determine how the user interacts with the framework

The end result should be to determine if you go to the game console 
*******************************************************************************************************/
bool MainMenu::display()const {
	int option = 0;
	bool GoToConsole = true;
	std::string name;
	std::cout << "---MAIN MENU---" << std::endl;
	std::cout << "\t 1.) NEW GAME" << std::endl;
	std::cout << "\t 2.) LOAD GAME" << std::endl;
	
	
	std::cout << "\n What would you like to do?: ";
	std::cin >> option;
	Game& gameInstance = Game::getinstance();
	switch (option) {

	case 1: {
		std::cout << " What name will you bestow upon your new character?: ";
		std::cin >> name;

		
		gameInstance.createPlayer(name);
		if (!gameInstance.PrePlay()) {
			GoToConsole = false;
		}
	}
		  break;
	case 2: {
		if (!gameInstance.loadGame()) {
			GoToConsole = false;
		}
		
		if (!SQLCONN::createInstance().loadPlayerData(gameInstance.playerN.getName())) {
			GoToConsole = false;
		}
	}
		  break;
	default:
		std::cout << "Please enter valid number" << std::endl;
		break;
	}

	return GoToConsole;
}
//map()->loadMapData list of all maps with avail bosses and levels// location is chosen and paths are created and filled

/******************************************************************************************************
MAP CLASS init,cleaner, getters and setters

*******************************************************************************************************/
Map::Map() {
	loadMapData();
	findXpCap(Game::getinstance().playerN.getLvl());
}
Map::~Map() {}

/******************************************************************************************************
MAP CLASS deterministic functions: creates maps based on certain flags and data inherited from player (lvl,
currentLocation)

*******************************************************************************************************/
//int 
std::unique_ptr<int>determinenextconnectedPath(int path, int locationSize) {

	std::map<int, int>& checker = Game::getinstance().nextPathTracker;

	if (checker.find(path) != checker.end() && checker[path] > 3) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distrib(0, locationSize - 1);
		return determinenextconnectedPath(distrib(gen), locationSize);
	}

	checker[path]++;
	return std::make_unique<int>(path);

}
void Map::upDateTracker() {
	if (!checkVisited(currentPlacement)) {
		addEdge(currentPlacement, nextPlacement);
	}
}
bool Map::checkVisited(int current) {
	q.push(1);
	visited[1] = 1;
	while (!q.empty()) {
		int curr = q.front();
		q.pop();
	/*	if (curr == current) {
			return true;
		}*/
		for (auto neigh : MoveTracker[curr]) {
				if (visited[neigh] != 1) {
					q.push(neigh);
					visited[neigh] = 1;
				}
		}
		
	}
	return false;
}
void Map::addEdge(int u, int v) {
	MoveTracker[u].push_back(v);
	MoveTracker[v].push_back(u);
}
void Map::displayTracker() {
	std::queue<int>tempq;
	std::map<int, int>vis;

	tempq.push(1);
	visited[1] = 1;
	while (!tempq.empty()) {
		int curr = tempq.front();
		tempq.pop();
		std::cout << curr << "\t";
		
		for (auto neighbor : MoveTracker[curr]) {
			std::cout << neighbor << std::endl;
			if (vis[neighbor] != 1) {
				tempq.push(neighbor);
				vis[neighbor] = 1;
			}
		}
	}
}
void Map::createPaths(int Location) {
	int connections = 0;
	if (Location <= 2) {
		end = SMALL_MAP;
		connections = 9;
	}
	else if (Location > 2 && Location <=4) {
		end = MEDIUM_MAP;
		connections = 11;
	}
	else if (Location > 4 && Location <= 6) {
		end = LARGE_MAP;
		connections = 13;
	}
	else {
	}
	int first = 0, second = 0;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distrib(1, end - 1); // Avoiding 0 as a possible value

		second = distrib(gen);
		if (second == 1) {
			second = 2;
		}
		add(1, second);
		std::cout << "(1" << " , " << second << ")" << std::endl;

		for (int i = 1; i < connections; ++i) {
			first = distrib(gen);
			second = distrib(gen);

			while (first == 0 || first == second || mapp[first].size() > 0 && std::find(mapp[first].begin(), mapp[first].end(), second) != mapp[first].end()) {
				first = distrib(gen);
				second = distrib(gen);
			}

			if (first != second) {
				if (std::find(mapp[first].begin(), mapp[first].end(), second) == mapp[first].end()) {
					add(first, second);
					std::cout << "(" <<first<< " , " << second << ")" << std::endl;
				}
			}
			else {
				if (first + 1 < end) {
					if (std::find(mapp[first].begin(), mapp[first].end(), first + 1) == mapp[first].end()) {
						add(first, first + 1);
						Game::getinstance().nextPathTracker[first + 1]++;
					}
				}
				if (std::find(mapp[first].begin(), mapp[first].end(), first - 1) == mapp[first].end()) {
					add(first, first - 1);
					Game::getinstance().nextPathTracker[first - 1]++;
				}
			}

			if (std::find(mapp[first].begin(), mapp[first].end(), second) == mapp[first].end()) {
				add(first, second);
				std::cout << "(" << first << " , " << second << ")" << std::endl;
			}
		}

		first = distrib(gen);
		while (first == 0 || first == second || mapp[first].size() > 0 && std::find(mapp[first].begin(), mapp[first].end(), end) != mapp[first].end()) {
			first = distrib(gen);
		}
		second = end;

		if (std::find(mapp[first].begin(), mapp[first].end(), second) == mapp[first].end()) {
			add(first, second);
			
		}
		std::cout << "(" << first << " , " << second << ")" << std::endl;
		loadPathway(SMALL_MAP);


	std::cout << "created!" << std::endl;
}
void Map::loadMapData() {
	//locationData.insert(std::pair<int, std::list<int,int,int>(1, 3)); //first island has 3 "dungeons"
	//this loads up all the maps
	locationData[1].push_back(0);
	locationData[1].push_back(0);
	locationData[1].push_back(1);

	locationData[2].push_back(0);
	locationData[2].push_back(1);
	locationData[2].push_back(1);
	
	locationData[3].push_back(0);
	locationData[3].push_back(0);
	locationData[3].push_back(1);

	locationData[4].push_back(0);
	locationData[4].push_back(0);
	locationData[4].push_back(1);

	locationData[5].push_back(0);
	locationData[5].push_back(0);
	locationData[5].push_back(1);

	locationData[6].push_back(0);
	locationData[6].push_back(0);
	locationData[6].push_back(1);

	locationData[7].push_back(0);
	locationData[7].push_back(0);
	locationData[7].push_back(1);

	locationData[8].push_back(0);
	locationData[8].push_back(0);
	locationData[8].push_back(1);

	locationData[9].push_back(0);
	locationData[9].push_back(0);
	locationData[9].push_back(1);
	//locations.push_back(locationData);
}
void Map::findXpCap(int currentLvl) {
	//used to stop the increase of XP until lvl up requirements met
	switch (currentLvl) {
	case 1:
		XpCap = 200;
		break;
	case 2:
		XpCap = 450;
		break;
	case 3:
		XpCap = 680;
		break;
	default:
		std::cout << "Ran into an error calculating xpCap" << std::endl;
		break;
	}
}
void Map::add(int s, int d) {
	mapp[s].push_back(d);
	mapp[d].push_back(s);
}
void EnemyAttacks(Character& p1, Character& en) {
	int enemyatk = en.attack();
	//player attempts to dodge
	int battleroll = rand() % 6 + 1;
	int damageDone = 0;
	if (p1.getDodge() != battleroll) {
		damageDone = (enemyatk - p1.getDef()) > 0 ? (enemyatk - p1.getDef()): 0;
		p1.setHP(p1.getHP() - damageDone);
		std::cout << "You were too slow. You've received " << damageDone << " damage." << std::endl;
	}
	else {
		std::cout << "You've dodged the attack" << std::endl;
	}

}

void PlayerAttacks(Character& p1, Character& en) {
	int playeratk = p1.attack();
	//player attempts to dodge
	int battleroll = rand() % 6 + 1;
	int damageDone = 0;
	//enemy attempts to dodge
	battleroll = rand() % 6 + 1;
	if (en.getDodge() != battleroll) {
		damageDone = (playeratk - en.getDef()) > 0 ? (playeratk - en.getDef()) :0 ;
		en.setHP(en.getHP() - damageDone);
		std::cout << "You're attack strikes true. " << damageDone << " damage dealt. " << std::endl;
	}
	else {
		std::cout << "You're attack was dodged" << std::endl;
	}
}

void BossAttacks(Character& p1, Boss& boss) {
	//this will later hold special attacks
	int bossatk = boss.attack();
	//player attempts to dodge
	int battleroll = rand() % 6 + 1;
	int damageDone = 0;
	if (p1.getDodge() != battleroll) {
		damageDone = (bossatk - p1.getDef());
		p1.setHP(p1.getHP() - damageDone);
		std::cout << "You were too slow. You've received " << damageDone << " damage." << std::endl;
	}
	else {
		std::cout << "You've dodged the attack" << std::endl;
	}
}
void DisplayRoundData(Character& p1, Character& en, int roundNum) {
	//DISPLAY CURRENT ROUND DATA
	std::cout << "***** ROUND: " << roundNum << "*****" << std::endl;
	std::cout << p1.getName() << " current HP: " << p1.getHP() << std::endl;
	std::cout << en.getName() << " current HP: " << en.getHP() << std::endl;
	std::cout << "********************************" << std::endl;
}
bool Map::bossBattle(int loc,int dunNum, Player& p1) {

	Boss newBoss(loc);
	SQLCONN& bossload = SQLCONN::createInstance();
	if (!bossload.getBoss(loc, dunNum, newBoss)) {
		std::cout << "Failed to load boss" << std::endl;
		return false;
	}
	int currentRound = 1;
	srand(time(NULL));

	std::cout << (p1.getSpd() <= newBoss.getSpd() ? newBoss.getName() + " is faster!" : "Your speed is greater") << std::endl;

	while ((p1.getHP() > 0) && (newBoss.getHP() > 0)) {
		if (p1.getSpd() <= newBoss.getSpd()) {
			// Enemy attacks first
			BossAttacks(p1, newBoss);
			PlayerAttacks(p1, newBoss);
		}
		else {
			// Player attacks first
			PlayerAttacks(p1, newBoss);
			if (newBoss.getHP() > 0) { //no boss attack from beyond the grave unless players health is less than their instant kill amount *tb added later
				BossAttacks(p1, newBoss);
			}
		}

		DisplayRoundData(p1, newBoss, currentRound);
		currentRound++;
	}
	if (p1.getHP() > 0) {
		std::cout << "You've triumphed over " << newBoss.getName() << std::endl;
		std::cout << "You're achievement: +" << newBoss.getGold() << " gold and +" << newBoss.getXP() << " XP" << std::endl;
		totalXP += newBoss.getXP();
		totalGold += newBoss.getGold();
		p1.setGold(p1.getGold() + totalGold);
		if (XpCap > p1.getXP()) {
			p1.setXP(p1.getXP() + totalXP);
		}
		else {
			std::cout << "You've reached the maximum amount of XP you can gain at this level. Complete remaining requirements to level up! \n";
		}
		

		auto it = Game::getinstance().playerN.Lvlrequirements.find(newBoss.getName()); //this is to find if the current beaten boss is part of the lvl up req
		if (it != Game::getinstance().playerN.Lvlrequirements.end()) {
			//we found it as a req
			std::cout << "You've completed the level up requirement. Evaluating....\n";
			it->second = true;
		}
		return true;
	}
	std::cout << "You've fallen in battle" << std::endl;
	return false;
}
std::unique_ptr<bool>Map::DungeonBattle(Player& pl, std::unique_ptr<Enemy>& en) {

	int currentRound = 1;
	srand(time(NULL));

	std::cout << (pl.getSpd() <= en->getSpd() ? en->getName() + " is faster!" : "Your speed is greater") << std::endl;

	while ((pl.getHP() > 0) && (en->getHP() > 0)) {
		if (pl.getSpd() <= en->getSpd()) {
			// Enemy attacks first
			EnemyAttacks(pl, *en);
			PlayerAttacks(pl, *en);//player can land a kill from the grave attack- 
		}
		else {
			// Player attacks first
			PlayerAttacks(pl, *en);
			if (en->getHP() > 0) {//enemy can only retaliate if they're alive...
			   EnemyAttacks(pl, *en);
			}
			
		}

		DisplayRoundData(pl, *en, currentRound);
		currentRound++;
	}

	if (pl.getHP() > 0) {
		std::cout << "You've triumphed over " << en->getName() << std::endl;
		return std::make_unique<bool>(true);
	}
	std::cout << "You've fallen in battle" << std::endl;
	return std::make_unique<bool>(false);
}


int Map::inGameInputs(std::vector<int>a) {
	int movement = 0;
	std::string input;

	std::cout << "\nWhat would you like to do?: ";

	while (true) {
		if (std::cin >> movement) {
			// Handle integer input
			// Perform actions based on the integer input (if needed)
			if ((movement <= 8)&&(movement > 0)) {
				std::vector<int>::iterator temp;
				auto it = find(a.begin(), a.end(), movement);
				if (it != a.end()) {
					break;// Exit the loop if an integer input is received and is part of the displayed moves available
				}
				
			}
			std::cout << "The attempted movement is invalid. Try again. ";
			
		}
		else {
			// Clear input stream state and discard invalid input
			std::cin.clear();
			std::cin >> input;

			// Handle string input
			if (input == "map") {
				if (nextPlacement != 0) {
					displayTracker();
				}
				else {
					std::cout << "You haven't made enough moves to show the map" << std::endl;
				}
			}
			else if (input == "inv") {
				Game::getinstance().displayInventory();
			}
			else if (input == "stats") {
				Game::getinstance().playerN.displayStats(false);
			}
			else {
				std::cout << "The input given wasn't a recognized option" << std::endl;
			}

			std::cout << "\nWhat would you like to do?: ";
		}
	}

	return movement;
}

int Map::availableMoves(int a) {
	srand(time(NULL));
	currentPlacement = a;

	if (pathwayy[a]) {//this gets the .second of the map which is whether there is an enemy
		//std::cout << "There is an enemy in the room" << std::endl;
		int listsize = Game::getinstance().enemyList.size();
		int randomEnemy = rand() % listsize+1;
		
		Enemy newEnemy(currentDungeonNum);
		if (randomEnemy >=listsize) {
			newEnemy = Game::getinstance().enemyList[randomEnemy - 1];
		}
		else {
			newEnemy = Game::getinstance().enemyList[randomEnemy];
		}
		
		

		//std::unique_ptr<Player> playerPtr = std::make_unique<Player>(Game::getinstance().playerN);
		std::unique_ptr<Enemy> newEnemyPtr = std::make_unique<Enemy>(newEnemy);
		std::unique_ptr<bool> battleResultPtr = DungeonBattle(Game::getinstance().playerN, newEnemyPtr);

		//std::cout << newEnemy.getName()<<" has appeared for battle" << std::endl;
		
		if (!(*battleResultPtr)) {
			return -1;
		}
		std::cout << "You're achievement: +" << newEnemy.getGold() << " gold and +" << newEnemy.getXP() << " XP" << std::endl;
		totalXP += newEnemy.getXP();
		totalGold += newEnemy.getGold();
	}
	//InGameDecisions(std::cin);
	std::vector<int>availableMoveList;
	std::cout << "The following pathways are available from here: ";
		for (auto neighbor : mapp[a]) {
			std::cout << "[ " << neighbor << " ";
			availableMoveList.push_back(neighbor);
		}
		std::cout << "]";
		
		int temp= inGameInputs(availableMoveList);
		availableMoveList.clear();
		nextPlacement = temp;
		upDateTracker();
		return temp;
}
void Map::makeMove(int currLocation) {
	if (currLocation == end) {
		return;
	}
	if (currLocation == -1) {
		return;
	}
	makeMove(availableMoves(currLocation));

}

void Map::loadPathway(int n) {//a list of which path has an enemy present
	srand(time(NULL));
	int reachThreeCount = 1;
	for (int i = 1; i <= n; ++i) {
		bool hasEnemy = rand() % 2;
		if (reachThreeCount % 3==0) {
			hasEnemy = true;
		}
		pathwayy.insert(std::pair<int, bool>(i, hasEnemy));
		if (!hasEnemy) {
			reachThreeCount++;
		}
		
	}
}



/******************************************************************************************************
STORE ITEM CLASS init,cleaner, getters and setters

*******************************************************************************************************/
Item::Item() {
	isEquipped = false;
	price = 0;
	strBuff = 0;
	defBuff = 0;
	HPBuff = 0;
	Requirement = 0;
	itemID = 0;
}
Item::~Item() {

}
bool Item::getIsEquip()const {
	return isEquipped;
}
std::string Item::getItemName()const {
	return itemName;
}
void Item::setName(std::string n) {
	itemName = n;
}
int Item::getPrice() const {
	return price;
}
void Item::setPrice(int p) {
	price = p;
}
void Item::setType(std::string t) {
	itemType = t;
}
std::string Item::getType()const {
	return itemType;
}
std::pair<std::string, int> Item::getStats()const {
	std::pair<std::string, int>buffs;
	if (strBuff == 0 && defBuff == 0) {
		buffs = std::make_pair("HP", HPBuff);
		return buffs;
	}
	if (defBuff == 0 && HPBuff == 0) {
		buffs = std::make_pair("Str", strBuff);
		return buffs;
	}
	buffs = std::make_pair("Def", defBuff);
	return buffs;
}
void Item::setRequirement(int req) {
	Requirement = req;
}
int Item::getRequirement()const {
	return Requirement;
}
void Item::setItemStats(int s, int d, int h) {
	strBuff = s;
	defBuff = d;
	HPBuff = h;
}
void Item::set_EquipON() {
	isEquipped = true;
}
void Item::set_EquipOff() {
	isEquipped = false;
}
int Item::getitemID()const {
	return itemID;
}
void Item::setID(int iD) {
	itemID = iD;
}
bool Item::operator==(const Item& o)const {
	return this->itemID == o.itemID;
}
/******************************************************************************************************
GAME CONSOLE CLASS init,cleaner, getters and setters

*******************************************************************************************************/

GameConsole::GameConsole() {

}
GameConsole::~GameConsole() {}

/******************************************************************************************************
GAME CONSOLE deterministic functions: travel, go to store, back to main, display stats, display curr data, 
inventory

*******************************************************************************************************/
void GameConsole::display()const {
	std::cout << "\n Name: " << Game::getinstance().playerN.getName() << "\t";
	std::cout << "Level: " << Game::getinstance().playerN.getLvl() << std::endl;
	std::cout << "Health: " << Game::getinstance().playerN.getHP() << "\t";
	std::cout << "Current Location: " << Game::getinstance().playerN.location<<std::endl;
	std::cout << "XP: " << Game::getinstance().playerN.getXP() << std::endl;
	std::cout << "Gold: " << Game::getinstance().playerN.getGold() << std::endl;

}
void GameConsole::options() {
	int option;
	bool displayOptions = true;
	Game& currentGame = Game::getinstance();
	while (displayOptions) {
		//check if there is a level up
		if (currentGame.playerN.can_level_up()) {
			std::cout << "Congratulations! You've leveled up to : " << currentGame.playerN.getLvl() << '\n';
			std::cout << "Check your new stats by choosing display all from the below options \n";
		}

		std::cout << "\n1.) Travel" << std::endl;
		std::cout << "2.) Enter Mission" << std::endl;
		std::cout << "3.) Go to Inventory" << std::endl;
		std::cout << "4.) Go to Store" << std::endl;
		std::cout << "5.) Display All Stats" << std::endl;
		std::cout << "6.) Save Game" << std::endl;
		std::cout << "7.) Back to Main Menu" << std::endl;
		std::cout << "8.) Quit" << std::endl;

		std::cout << "Which event do you choose to complete?: ";
		if (std::cin >> option) {

			switch (option) {
			case 1:
				//shows available islands
			{
				int placeToTravel = currentGame.TravelonWorldMap();
				if (placeToTravel > 0) {
					currentGame.playerN.location = placeToTravel;
					currentGame.getLocationName(placeToTravel);
				}
			}
				break;
			case 2:
				//shows islands quests
				currentGame.displayMapsAvailable();
				currentGame.startMission();

				break;
			case 3:
				currentGame.displayInventory();
				break;
			case 4:
				currentGame.loadStore();
				currentGame.displayStore(currentGame.playerN.getLvl());
				break;
			case 5:
				display();
				currentGame.playerN.displayStats(true);
				break;
			case 6:
				saveState();

				break;
			case 7:
				backtoMain();
				break;
			case 8:
				return;
			default:
				std::cerr << "Please choose an appropriate option from above" << std::endl;
				break;
			}
		}
		else {
			std::cout << "\n That wasnt a valid option" << std::endl;
			options();
		}
	}
}
void GameConsole::backtoMain() {
	//call main menu instance
	MainMenu::getInstance().display();
}

void GameConsole::saveState() {
	int choice = 0;
	std::cout << "How would you like to save your data?: " << std::endl;
	std::cout << " 1.) Save to database " << std::endl;
	std::cout << " 2.) Save locally " << std::endl;
	std::cin >> choice;
	switch (choice) {
	case 1:
		if (!SQLCONN::createInstance().sqlSave()) {
			
			std::cout << "failed to save data" << std::endl;
		}
		if ((!SQLCONN::createInstance().InvSave())) {
			std::cout << "failed to save inventory data \n";
		}
		break;
	case 2:
		savedData::getInstance().uploadData(Game::getinstance().playerN);
		break;
	default:
		std::cerr << "Please choose an appropriate option from above" << std::endl;
		break;
	}
}
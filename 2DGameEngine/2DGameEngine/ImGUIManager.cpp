#include <sstream>
#include <iostream>
#include <fstream>
#include <ImGuiFileDialog.h>
#include "ImGUIManager.h"
#include "GameObject.h"
#include "GameObjectActions.h"
#include "InputManager.h"
#include "Game.h"

ImGUIManager::ImGUIManager(TextureManager* texM, TerrainManager* terrM, GameObjectManager* goM, int screenW, int screenH) {
	_textureManager = texM;
	_terrainManager = terrM;
	_gameObjectManager = goM;
	_screenWidth = screenW;
	_screenHeight = screenH;
}

ImGUIManager::~ImGUIManager() {
}

void ImGUIManager::setInputManager(InputManager* inputM) {
	_inputManager = inputM;
}

void ImGUIManager::displayImGUI() {
	static int xi = 0, yi = 0, wi = 150, hi = 150;
	static ImVec4 colour = ImVec4(0.4f, 0.0f, 0.4f, 1.0f);
	
	_selectedGOIndex = _inputManager->getCurrentGOIndex();
	_toolsWindow();
	_textureWindow();
	//_gameObjectsWindow();
	_selectedGOWindow();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGUIManager::saveProject(std::string filename) {

	std::ofstream savefile("saves/" + filename);

	if (savefile.fail()) {
		return false;
	}

	std::vector<Texture2D*> textures = _textureManager->getTextureList();
	std::vector<TerrainTexture*> terrainTextures = _terrainManager->getTerrainList();
	std::vector<GameObject*> gameObjects = *(_gameObjectManager->getGameObjectList());
	std::vector<GameObject*> terrainObjects = *(_gameObjectManager->getTerrainObjectList());

	savefile << "textures {\n";
	int i = 0;
	for (Texture2D* te : textures) {
		savefile << "\t" << i << _sep << te->getPath() << "\n";
		i++;
	}
	savefile << "}\n";

	savefile << "terrainTextures {\n";
	i = 0;
	for (TerrainTexture* te : terrainTextures) {
		savefile << "\t" << i << _sep << te->getPath() << "\n";
		i++;
	}
	savefile << "}\n";

	savefile << "gameObjects {\n";
	i = 0;
	for (GameObject* go : gameObjects) {
		glm::vec2 pos = go->getPosition();
		glm::vec2 size = go->getSize();
		savefile << "\t" << i << _sep << go->getName() << _sep << pos.x << _sep << pos.y << _sep << size.x << _sep << size.y << _sep << _textureManager->getTextureIndex(go->getTexture());
		savefile << _sep << go->getColourAsString() << _sep << go->canMove() << _sep << go->hasCollision() << _sep << go->hasTerrainCollision() << _sep << go->getAction();
		savefile << _sep << go->getActionIterations() << _sep << go->getActionMovSpeed() << _sep << go->getCurrentAutoMoveDir() << _sep << go->isGenerator();
		savefile << _sep << go->getGenerationRate() << _sep << go->isBlocker() << _sep << go->isExit() << "\n";
		i++;
	}
	savefile << "}\n";

	savefile << "terrainObjects {\n";
	i = 0;
	for (GameObject* to : terrainObjects) {
		glm::vec2 pos = to->getPosition();
		glm::vec2 size = to->getSize();

		savefile << "\t" << i << _sep << pos.x << _sep << pos.y << _sep << size.x << _sep << size.y << _sep << _terrainManager->getTerrainIndex(to->getTexture()) << "\n";
		i++;
	}

	savefile << "}\n";

	savefile.close();
	return true;	
}

bool ImGUIManager::loadProject(std::string filename) {
	std::ifstream savefile("saves/" + filename);
	if (savefile.fail()) {
		std::cout << "Failed opening file" << std::endl;
		return false;
	}

	std::string line;
	std::getline(savefile, line);
	if (line != "textures {")	return false;

	while (std::getline(savefile, line)) {
		if (line == "}") break;
		std::vector<std::string> entries = _split(line, _sep);
		if (entries.size() != 2) return false;
		if (nullptr == _textureManager->loadNewTexture(entries[1])) {
			std::cout << "Failed loading texture " << entries[1] << std::endl;
			return false;
		}
	}

	std::cout << "Loaded all textures ---------------------" << std::endl;

	std::getline(savefile, line);
	if (line != "terrainTextures {")	return false;

	while (std::getline(savefile, line)) {
		if (line == "}") break;
		std::vector<std::string> entries = _split(line, _sep);
		if (entries.size() != 2) return false;
		if (nullptr == _terrainManager->loadNewTerrain(entries[1])) {
			std::cout << "Failed loading terrainTexture " << entries[1] << std::endl;
			return false;
		}
	}

	std::cout << "Loaded all terrain textures ---------------------" << std::endl;

	std::getline(savefile, line);
	if (line != "gameObjects {")	return false;

	while (std::getline(savefile, line)) {
		if (line == "}") break;
		std::vector<std::string> entries = _split(line, _sep);
		if (entries.size() != 19) {
			std::cout << "Wrong size for the properties of a gameObject, expected 19, actual: " << entries.size() << std::endl;
			return false;
		}

		GameObject* go = new GameObject();
		go->setName(entries[1]);

		glm::vec2 pos(std::stoi(entries[2]), std::stoi(entries[3]));
		glm::vec2 size(std::stoi(entries[4]), std::stoi(entries[5]));
		go->init(pos, size, 0.0f, glm::vec3(1.0f));
		int texIndex = std::stoi(entries[6]);
		if (texIndex != -1) {
			Texture2D* tex = _textureManager->getTexture(texIndex);
			if (tex == nullptr) {
				std::cout << "Expected texture was not found. Aborting load..." << std::endl;
				return false;
			}
			go->setTexture(tex);
		} else {
			go->setTexture(nullptr);
		}

		go->setColour(std::stoul(entries[7]));
		go->setCanMove(entries[8] == "1");
		go->setHasCollision(entries[9] == "1");
		go->setHasTerrainCollision(entries[10] == "1");
		go->setAction(std::stoi(entries[11]));
		go->setActionIterations(std::stoi(entries[12]));
		go->setActionMovSpeed(std::stoi(entries[13]));
		int autoMovDir = std::stoi(entries[14]);
		if (autoMovDir != -1) {
			go->setAutoMoveDir(autoMovDir, true);
		}
		go->setIsGenerator(entries[15] == "1");
		go->setGenerationRate(std::stoi(entries[16]));
		go->setIsBlocker(entries[17] == "1");
		go->setIsExit(entries[18] == "1");
		_gameObjectManager->addGameObject(go);
		if (go->isBlocker() || go->isExit()) _gameObjectManager->setBlockingObject(go);
	}

	std::cout << "Loaded all gameobjects ---------------------" << std::endl;

	std::getline(savefile, line);
	if (line != "terrainObjects {")	return false;

	while (std::getline(savefile, line)) {
		if (line == "}") break;
		std::vector<std::string> entries = _split(line, _sep);
		if (entries.size() != 6) return false;
		TerrainTexture* terr = _terrainManager->getTerrainList().at(std::stoi(entries[5]));
		_gameObjectManager->setTerrainObjectTexture(terr);
	}

	std::cout << "Loaded all terrain Objects ---------------------" << std::endl;
	std::cout << "SUCCESS" << std::endl;

	return true;
}

void ImGUIManager::clearEverything() {
	//Remove everything from the scene, to either start anew or to load a saved project
	_selectedGOIndex = -1;
	_selectedTexIndex = 0;
	_selectedTerrIndex = 0;
	_inputManager->setCurrentGOIndex(-1);
	_gameObjectManager->clearVectors();
	_terrainManager->clearTerrainVector();
	_textureManager->clearTextureVector();
}

void ImGUIManager::_toolsWindow() {
	ImGui::Begin("Tools");
	static int state = 0;

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::Separator();
	ImGui::Text("Game Object");	ImGui::SameLine(); 
	ImGui::Text("(?)");
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("What should happen when clicking on the game environment");
	ImGui::RadioButton("Select", &state, InputManager::SELECT_OBJECT); 

	ImGui::SameLine();
	ImGui::RadioButton("Add", &state, InputManager::ADD_OBJECT);	

	if (state != _inputManager->getClickState())
		_inputManager->setClickState(state);

	ImGui::Separator();
	std::string gameStatus = Game::isPaused ? "Game paused" : "Game running";
	ImGui::Text(gameStatus.c_str());

	std::string gameStateButton = Game::isPaused ? "Continue game" : "Pause game";
	if (ImGui::Button(gameStateButton.c_str())) Game::isPaused = !Game::isPaused;
	ImGui::Separator();

	if (ImGui::Button("Clear everything")) clearEverything();
	ImGui::SameLine();
	ImGui::Text("(?)");
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Removes all GameObjects, TerrainTextures and Texture2D instances from the game environment.");

	ImGui::Separator();

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.40f);

	static char str0[128] = "save1.save";
	static bool success = false;
	static bool pressed = false;
	ImGui::InputText("Save filename", str0, IM_ARRAYSIZE(str0)); 

	if (ImGui::Button("Save project")) {
		success = saveProject(str0);
		pressed = true;
	}

	if (ImGui::Button("Load project")) {
		success = loadProject(str0);
		pressed = true;
	}

	if (pressed) {
		ImGui::SameLine();
		ImGui::Text(success ? "Success" : "Failed");
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void ImGUIManager::_textureWindow() {
	ImGui::Begin("Textures");

	static bool isTerrain = false;

	if (ImGui::Button("Load texture")) {
		isTerrain = false;
		igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".jpg\0.jpeg\0\0", ".");
	}

	ImGui::SameLine();

	if (ImGui::Button("Load terrain texture")) {
		isTerrain = true;
		igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".png\0", ".");
	}

	if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey")) {
		if (igfd::ImGuiFileDialog::Instance()->IsOk == true) {
			std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilepathName();
			if (isTerrain) {
				_terrainManager->loadNewTerrain(filePathName);
			} else {
				_textureManager->loadNewTexture(filePathName);
			}
		}
		igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
	}

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

	if (_textureManager->getTextureList().size() > 0) {
		std::vector<std::string> textureNames = _textureManager->getTextureNames();
		const char* currentTex = textureNames[_selectedTexIndex].c_str();

		if (ImGui::BeginCombo("Loaded texture list", currentTex)) {
			for (int n = 0; n < static_cast<int>(textureNames.size()); n++) {
				bool is_selected = (strcmp(currentTex, textureNames[n].c_str()) == 0);
				if (ImGui::Selectable(textureNames[n].c_str(), is_selected))
					_selectedTexIndex = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Apply to selected object"))
			_gameObjectManager->setObjectTexture(_selectedGOIndex, _textureManager->getTexture(_selectedTexIndex));

	}

	ImGui::Separator();

	if (_terrainManager->getTerrainList().size() > 0) {
		std::vector<std::string> terrainNames = _terrainManager->getTerrainNames();
		const char* currentTerr = terrainNames[_selectedTerrIndex].c_str();

		if (ImGui::BeginCombo("Loaded terrain list", currentTerr)) {
			for (int n = 0; n < static_cast<int>(terrainNames.size()); n++) {
				bool selected = (strcmp(currentTerr, terrainNames[n].c_str()) == 0);
				if (ImGui::Selectable(terrainNames[n].c_str(), selected))
					_selectedTerrIndex = n;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Make current terrain"))
			_gameObjectManager->setTerrainObjectTexture(_terrainManager->getTerrain(_selectedTerrIndex));

	}


	ImGui::End();	
}

void ImGUIManager::_gameObjectsWindow() {
	static int x = 0, y = 0, w = 150, h = 150;

	ImGui::Begin("Game objects");

	if (_selectedGOIndex >= 0) {
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.70f);

		std::vector<std::string> objectNames = _gameObjectManager->getGameObjectNames();
		const char* currentItem = objectNames[_selectedGOIndex].c_str();

		if (ImGui::BeginCombo("Current game object", currentItem)) {
			for (int n = 0; n < static_cast<int>(objectNames.size()); n++) {
				bool is_selected = (strcmp(currentItem, objectNames[n].c_str()) == 0);
				if (ImGui::Selectable(objectNames[n].c_str(), is_selected))
					_selectedGOIndex = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	ImGui::End();
}

void ImGUIManager::_selectedGOWindow() {
	//Fix bug when selected object gets deleted by the collision calculator
	if (_selectedGOIndex >= 0 && _selectedGOIndex < _gameObjectManager->getGameObjectList()->size()) {
		GameObject * go = _gameObjectManager->getGameObject(_selectedGOIndex);
		float mSpeed = go->getMovementSpeed();
		int width = go->getWidth();
		int height = go->getHeight();
		int posX = go->getPosition().x;
		int posY = go->getPosition().y;
		bool canMove = go->canMove();
		bool hasColl = go->hasCollision();
		bool terrColl = go->hasTerrainCollision();
		bool isBlocker = go->isBlocker();
		std::vector<bool> autoMoveDir = go->getAutoMoveDir();
		bool isExit = go->isExit();
		bool isGenerator = go->isGenerator();
		int action = go->getAction();
		float actionMovSpeed = go->getActionMovSpeed();
		int actionIter = go->getActionIterations();
		bool isPerfAction = go->isPerformingAction();
		double generatorRate = go->getGenerationRate();
		int generatorAmount = go->getGenerationAmount();
		glm::vec3 colour = go->getColour();

		ImGui::Begin("Game object properties");

		std::string text = "Current object: " + go->getName();
		ImGui::Text(text.c_str());

		if (ImGui::CollapsingHeader("Properties")) {
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.16f);

			if (ImGui::InputInt("Width", &width, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) go->setWidth(width); ImGui::SameLine();
			if (ImGui::InputInt("Height", &height, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) go->setHeight(height);
			ImGui::SameLine();
			ImGui::Text("(?)");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Please press ENTER while writing in the textbox to perform the modifications.");

			ImGui::Separator();

			ImGui::Text("Colour:"); ImGui::SameLine();
			float colourArray[3] = { colour.x, colour.y, colour.z };

			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.40f);
			ImGui::ColorPicker3("Colour", colourArray); 	go->setColour(glm::vec3(colourArray[0], colourArray[1], colourArray[2]));

			ImGui::Separator();

			ImGui::Checkbox("Can move", &canMove); 
			if (canMove != go->canMove()) go->setCanMove(canMove); ImGui::SameLine();

			ImGui::PopItemWidth(); ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.13f);

			if (ImGui::InputFloat("Movement speed", &mSpeed, 0, 0, "%.2f",ImGuiInputTextFlags_EnterReturnsTrue)) go->setMovementSpeed(mSpeed);
			
			ImGui::PopItemWidth();

			ImGui::Checkbox("Has blocker collision", &hasColl);
			if (hasColl != go->hasCollision()) go->setHasCollision(hasColl);

			ImGui::Checkbox("Has terrain collision", &terrColl);
			if (terrColl != go->hasTerrainCollision()) go->setHasTerrainCollision(terrColl);
		}

		if (ImGui::CollapsingHeader("Type")) {
			ImGui::Checkbox("Exit", &isExit);
			if (isExit != go->isExit()) {
				isExit ? _gameObjectManager->setBlockingObject(_selectedGOIndex)
					   : _gameObjectManager->removeBlockingObject(_selectedGOIndex);
				go->setIsExit(isExit);
			}
			ImGui::SameLine();
			ImGui::Text("(?)");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("GameObject needs to be set as a Blocker for the exit to work!");
								
			ImGui::SameLine();

			ImGui::Checkbox("Generator", &isGenerator);
			if (isGenerator != go->isGenerator()) go->setIsGenerator(isGenerator);

			if (isGenerator) {
				ImGui::Separator();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.24f);
				if (ImGui::InputDouble("Generation rate (in sec)", &generatorRate, 0, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) go->setGenerationRate(generatorRate);
				if (ImGui::InputInt("Generation amount", &generatorAmount, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) go->setGenerationAmount(generatorAmount);
			}
		}

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.16f);

		if (ImGui::CollapsingHeader("Auto movement")) {
			bool autoLeft = autoMoveDir[LEFT];
			bool autoRight = autoMoveDir[RIGHT];
			bool autoUp = autoMoveDir[UP];
			bool autoDown = autoMoveDir[DOWN];

			ImGui::Checkbox("Left", &autoLeft); ImGui::SameLine();
			if (autoLeft != autoMoveDir[LEFT]) go->setAutoMoveDir(LEFT, autoLeft);

			ImGui::Checkbox("Right", &autoRight); ImGui::SameLine();
			if (autoRight != autoMoveDir[RIGHT]) go->setAutoMoveDir(RIGHT, autoRight);

			ImGui::Checkbox("Up", &autoUp); ImGui::SameLine();
			if (autoUp != autoMoveDir[UP]) go->setAutoMoveDir(UP, autoUp);

			ImGui::Checkbox("Down", &autoDown);
			if (autoDown != autoMoveDir[DOWN]) go->setAutoMoveDir(DOWN, autoDown);

			if (ImGui::Button("Stop auto movement"))	go->stopAutoMove();
		}

		if (ImGui::CollapsingHeader("Lemmings actions")) {

			std::string actionText = "Current action: " + _getActionName(action);
			ImGui::Text(actionText.c_str());

			if (ImGui::InputFloat("Movement speed during action", &actionMovSpeed, 0, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) go->setActionMovSpeed(actionMovSpeed);
			if (ImGui::InputInt("Action iterations", &actionIter, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) go->setActionIterations(actionIter);
			
			ImGui::Separator();

			if (ImGui::Button("No action"))			go->setAction(NO_ACTION);	ImGui::SameLine();
			if (ImGui::Button("Build stairs"))		go->setAction(ACT_STAIRS);	ImGui::SameLine();
			if (ImGui::Button("Climber"))			go->setAction(ACT_CLIMB);

			if (ImGui::Button("Dig vertical"))		go->setAction(ACT_DIG_V);	ImGui::SameLine();
			if (ImGui::Button("Dig horizontal")) 	go->setAction(ACT_DIG_H);	ImGui::SameLine();			
			if (ImGui::Button("Dig diagonal")) 		go->setAction(ACT_DIG_DIAG);

			ImGui::Checkbox("Is Blocker", &isBlocker);
			if (isBlocker != go->isBlocker()) {
				isBlocker ? _gameObjectManager->setBlockingObject(_selectedGOIndex)
						  : _gameObjectManager->removeBlockingObject(_selectedGOIndex);
			} 

			ImGui::SameLine();

			if (ImGui::Button("Explode")) {
				_gameObjectManager->explodeGameObject(_selectedGOIndex);
				_inputManager->setCurrentGOIndex(-1);
			}
		}

		if (ImGui::Button("Delete object")) {
			_gameObjectManager->deleteGameObject(_selectedGOIndex);
			_inputManager->setCurrentGOIndex(-1);
		}

		ImGui::PopItemWidth();
		ImGui::End();		
	}
}

std::string ImGUIManager::_getActionName(int action) {
	switch (action) {
		case NO_ACTION: return "No action"; break;
		case ACT_DIG_H: return "Dig horizontal"; break;
		case ACT_DIG_V: return "Dig vertical"; break;
		case ACT_DIG_DIAG: return "Dig diagonal"; break;
		case ACT_STAIRS: return "Build stairs";	break;
		case ACT_CLIMB: return "Climber"; break;
		case ACT_GENERATE: return "Generator"; break;
		default: return ""; break;
	}
}

std::string ImGUIManager::_getFileName(const std::string& s) {
	//Returns the filename of the passed absolute path
	//Based on https://stackoverflow.com/a/46931770
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;
	char delim = '\\';

	while (std::getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result[result.size() - 1];
}

std::vector<std::string> ImGUIManager::_split(std::string s, std::string delimiter) {
	//Function taken from https://stackoverflow.com/a/46931770
	
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

bool ImGUIManager::_stringToBool(std::string s) {
	return false;
}

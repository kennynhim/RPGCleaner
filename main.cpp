#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

void displayMenu()
{
    std::cout << std::endl <<
    "* * * MAIN MENU * * *" << std::endl <<
    "1. Delete profiles" << std::endl <<
    "2. Collapse profiles" << std::endl <<
    "Q. Quit" << std::endl <<
    "Enter an option: ";
}

//getTotal returns the total number of player profiles to be deleted/collapsed as specified by the user
size_t getTotal(const size_t &targetLevels)
{
    size_t sum{0};

    std::ifstream tempF{"UT2004RPG.txt"};
    std::string line{};
    
    while (std::getline(tempF, line))
    {
        if (line.find("RPGPlayerDataObject") != std::string::npos)
        {
            std::getline(tempF, line);  //PlayerID
            std::getline(tempF, line);  //Player Level
            std::string level = line.substr(6, line.length()-6);
            std::istringstream iss{level};
            size_t levelInt{0};
            iss >> levelInt;
            if (levelInt <= targetLevels) 
                sum++;
        }
    }
    return sum;
}

void getConfirm(char& confirm, const int& j)
{
    switch (j)
    {
        case 1:
            std::cout << "Y - Yes, proceed" << std::endl;
            std::cout << "N - No, re-enter a number" << std::endl;
            break;
        case 2:
            std::cout << "Perform the transaction?" << std::endl;
            std::cout << "Y - Yes, perform transaction" << std::endl;
            break;
    }
    std::cout << "M - Return to main menu" << std::endl;
    std::cout << "Enter an option: ";
    std::cin >> confirm;
    while (std::toupper(confirm) != 'Y' && std::toupper(confirm) != 'N' && std::toupper(confirm) != 'M')
    {
        std::cout << "Select from an option above" << std::endl;
        std::cin.clear();
        std::cin.ignore(INT_MAX, '\n');
        std::cin >> confirm;
    }
}

std::string setAction(const char& operation)
{
    if (operation == '1')
        return "Delete";
    else if (operation == '2')
        return "Collapse";
    return "Unknown";
}

size_t getLevels(const char& operation, char& confirm)
{
    int levels{0};
    std::string action{setAction(operation)};
    
    confirm = 'N';
    while (std::toupper(confirm) == 'N')
    {
        std::cout << std::endl << "Below what level and under would you like to " << action << " player profiles?: ";
        while(!(std::cin >> levels))
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            std::cout << "You must enter a number" << std::endl;
        }
        while (levels < 0)
        {
            std::cout << "You must enter a positive number." << std::endl;
            while(!(std::cin >> levels))
            {
                std::cin.clear();
                std::cin.ignore(INT_MAX, '\n');
                std::cout << "You must enter a number" << std::endl;
            }
        }
        std::cout << "You want to " << action << " player profiles levels " << levels << " and under. Is this correct?" << std::endl;
        getConfirm(confirm, 1);
    }
    return levels;
}

size_t getSaveNum(const char& operation, char& confirm)
{
    int saveNum{0};
    std::string action{setAction(operation)};
    
    confirm = 'N';
        
    //Prompt the user for a non-zero integer to which the number of new player profiles should not be deleted
    while (std::toupper(confirm) == 'N')
    {
        std::cout << std::endl << "Of the player profiles to be " << action << "d, how many new profiles(ie those who recently joined the server) would you like to save?" << std::endl;
        std::cout << "If you don't want to save any new player profiles, enter 0: ";
        while (!(std::cin >> saveNum))
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            std::cout << "You must enter a number" << std::endl;
        }
        while (saveNum < 0)
        {
            std::cout << "You must enter a positive integer" << std::endl;
            while (!(std::cin >> saveNum))
            {
                std::cin.clear();
                std::cin.ignore(INT_MAX, '\n');
                std::cout << "You must enter a number" << std::endl;
            }
        }
            std::cout << "You want to save " << saveNum << " player profiles from being " << action << "ed. Is this correct?" << std::endl;
        getConfirm(confirm, 1);
    }
    return saveNum;
}

bool proceed(char& confirm, const size_t& levels, const size_t& saveNum, const char& operation)
{
    std::string action{setAction(operation)};
        
    std::cout << std::endl << "* * * REVIEW TRANSACTION * * *" << std::endl;
    std::cout << "Action: " << action << std::endl;
    std::cout << "Levels equal to and under: " << levels << std::endl;
    std::cout << "Number of new profiles to save: " << saveNum << std::endl;
    getConfirm(confirm, 2);
    
    if (std::toupper(confirm) == 'Y')
        return true;
    return false;
}

bool deleteProfiles(std::ifstream& inF, std::ofstream& outF, const size_t& deleteLevels, const size_t& saveNum, const size_t& deleteTotal)
{
    std::string line{}; //Stores current line of input stream
    size_t count{0};    //Current iteration of player profile the input stream is on
    size_t total{0};    //Sums the number of player profiles
    
    //Loop through the UT2004RPG.ini, with each iteration examining a line
    std::cout << std::endl << "* * * Deleting profiles... please wait. * * *" << std::endl;
    while (std::getline(inF, line))
    {
        bool bDelete{false};
        //If we have a player profile, we need to do some checks before we copy over lines
        if (line.find("RPGPlayerDataObject") != std::string::npos)
        {
            //We have a player profile here. We need to jump two lines down and check what level the player is
            //We'll do this by getLine, which extracts from the input stream
            
            //Store player's ID
            std::string playerIDLine{};
            std::getline(inF, playerIDLine);
            
            //Store player's level
            std::string playerLevelLine{};
            std::getline(inF, playerLevelLine);
            
            //playerLevelLine now holds the text "Level=x". We need to examine x
            std::string level = playerLevelLine.substr(6, playerLevelLine.length()-6);
            
            //Extract the level string and input it into an integer
            std::istringstream iss{level};
            size_t levelInt{0};
            iss >> levelInt;
            
            //Compare levelInt to requested deleteLevels
            //If levelInt <= deleteLevels, delete the player profile
            if (levelInt <= deleteLevels && count < (deleteTotal-saveNum))
            {
                count++;
                bDelete = true;
                //Skip past everything until line.length() == 0
                while (line.length() != 0)
                {
                    std::getline(inF, line);        //Advance line to the next line until we hit an empty string
                }
            }
            else    //Do not delete this profile. Copy over line, playerIDLine, and playerLevelLine
            {
                outF << line << std::endl << playerIDLine << std::endl << playerLevelLine << std::endl;
                std::getline(inF, line); //Move on to the next line and let loop take care of the rest of the profile
            }
            total++;
        }
        if (!bDelete)
            outF << line << std::endl;
    }
    std::cout << std::endl << "Deleted " << count << " out of " << total << " profiles." << std::endl << std::endl;
    return true;
}

bool collapseProfiles(std::ifstream &inF, std::ofstream &outF, const size_t& collapseLevels, const size_t& saveNum, const size_t& collapseTotal)
{
    std::string line{}; //Stores current line of input stream
    size_t count{0};    //Current iteration of player profile the input stream is on
    size_t total{0};    //Sums the number of player profiles
    
    //Loop through the UT2004RPG.ini, with each iteration examining a line
    std::cout << std::endl << "* * * Collapsing profiles... please wait. * * *" << std::endl;
    while (std::getline(inF, line))
    {
        bool bDelete{false};
        //If we have a player profile, we need to do some checks before we copy over lines
        if (line.find("RPGPlayerDataObject") != std::string::npos)
        {
            //We have a player profile here. We need to jump two lines down and check what level the player is
            //We'll do this by getLine, which extracts from the input stream
            
            //Store player's ID
            std::string playerIDLine{};
            std::getline(inF, playerIDLine);
            
            //Store player's level
            std::string playerLevelLine{};
            std::getline(inF, playerLevelLine);
            
            //playerLevelLine now holds the text "Level=x". We need to examine x
            std::string level = playerLevelLine.substr(6, playerLevelLine.length()-6);
            
            //Extract the level string and input it into an integer
            std::istringstream iss{level};
            size_t levelInt{0};
            iss >> levelInt;
            
            //Compare levelInt to requested collapseLevels
            //If levelInt <= collapseLevels, collapse the player profile
            if (levelInt <= collapseLevels && count < (collapseTotal-saveNum))
            {
                count++;
                //We need to keep 7 items when collapsing: Name, OwnerID, Level, Experience, NeededExp, Class, ClassLevel
                //Lets write what we have- Name, OwnerID, Level
                outF << line << std::endl << playerIDLine << std::endl << playerLevelLine << std::endl;
                
                //Get Experience and NeededExp... sometimes these are out of order
                bool bFoundExp{false};
                bool bFoundNeededExp{false};
                while (!bFoundExp || !bFoundNeededExp)
                {
                    std::getline(inF, line);
                    if (!bFoundExp && line.find("Experience=") != std::string::npos)
                    {
                        bFoundExp = true;
                        outF << line << std::endl;
                    }
                    if (!bFoundNeededExp && line.find("NeededExp") != std::string::npos)
                    {
                        bFoundNeededExp = true;
                        outF << line << std::endl;
                    }
                    //Include this here in case we run to end of player profile
                    if (line.length() == 0)
                        break;
                }
                if (line.length() == 0)
                {
                    outF << line << std::endl;
                    continue;
                }
                
                //We now need to get the class. Advance input stream until we get to the Abilities section
                //It's possible for a player to have no abilities, so we also search until we hit an empty string, ie end of profile
                while (line.find("Abilities=") == std::string::npos)
                {
                    std::getline(inF, line);
                    if (line.length() == 0)
                        break;
                }
                if (line.length() == 0)
                {
                    outF << line << std::endl;
                    continue;
                }
                
                //Make sure we are at ability section. Otherwise, it's the end of the profile and we can continue from there
                if (line.find("Abilities=") != std::string::npos)
                {
                    while (line.find("WeaponsMaster") == std::string::npos &&  //And keep looping until we hit a Class
                    line.find("AdrenalineMaster") == std::string::npos &&
                    line.find("MonsterMaster") == std::string::npos &&
                    line.find(".Class") == std::string::npos &&
                    line.find("Engineer") == std::string::npos &&
                    line.find("General") == std::string::npos &&
                    line.find("Necromancer") == std::string::npos &&
                    line.find("ClassicRPG") == std::string::npos)
                    {
                        std::getline(inF, line);
                        if (line.length() == 0)
                            break;
                    }
                    //Either line has a length, which means we have a class, or line has length 0 which means we are no longer on the player's profile
                    if (line.length() == 0)
                    {
                        outF << line << std::endl;
                        continue;
                    }
                    else //We have a player class
                    {
                        outF << line << std::endl;
                        
                        //Player has a class. Set AbilityLevels=1
                        outF << "AbilityLevels=1" << std::endl;
                        
                        //Done with this player profile. Move to the next player profile
                        while (line.length() != 0)
                        {
                            std::getline(inF, line);
                        }
                    }
                }
                outF << std::endl;
                bDelete = true;
            }
            else    //Do not collapse this profile. Copy over line, playerIDLine, and playerLevelLine
            {
                outF << line << std::endl << playerIDLine << std::endl << playerLevelLine << std::endl;
                std::getline(inF, line); //Move on to the next line and let loop take care of the rest of the profile
            }
            total++;
        }
        if (!bDelete)
            outF << line << std::endl;
    }
    std::cout << std::endl << "Collapsed " << count << " out of " << total << " profiles." << std::endl << std::endl;
    return true;
}

bool setup(std::ifstream& inF, std::ofstream& outF, const char& operation)
{
    char confirm{'N'};
    size_t actionLevels = getLevels(operation, confirm);
    if (std::toupper(confirm) == 'M')
        return false;   //Returning false means we loop the menu back to the user
    size_t saveNum = getSaveNum(operation, confirm);
    if (std::toupper(confirm) == 'M')
        return false;
    if (!proceed(confirm, actionLevels, saveNum, operation))
        return false;
    size_t totalLevels = getTotal(actionLevels);
    if (operation == '1')
        return deleteProfiles(inF, outF, actionLevels, saveNum, totalLevels);
    else if (operation == '2')
        return collapseProfiles(inF, outF, actionLevels, saveNum, totalLevels);
    else
        return true;
}

int main()
{
    std::cout << "* * * * * " << "Welcome to the UT2004RPG.ini cleaner!" << " * * * * *" << std::endl;
    
    std::ofstream fileOUT{"UT2004RPGOUT.txt"};
    std::ifstream fileIN{"UT2004RPG.txt"};
    
    //If the UT2004RPG.ini does not exist, print an error message and terminate
    if (!fileIN)
    {
        std::cout << "Could not locate UT2004RPG.txt" << std::endl;
        std::cout << "Make sure UT2004RPG.txt is located in the same location as this program." << std::endl;
        std::cout << "Enter any key to exit" << std::endl;
        std::cin.get();
        return 0;
    }
    char selection{'0'};
    while (std::toupper(selection) != 'Q')
    {
        displayMenu();
        std::cin >> selection;
        std::cin.ignore(INT_MAX, '\n');
        if (selection != '1' && selection != '2' && std::toupper(selection) != 'Q')
            std::cout << "Please select 1, 2, or Q from above" << std::endl;
        else if (std::toupper(selection) == 'Q' || setup(fileIN, fileOUT, selection))
            break;
    }
    fileIN.close();
    fileOUT.close();
    if (std::toupper(selection) != 'Q')
    {
        std::cout << "A new text document called UT2004RPGOUT has been made with your changes." << std::endl << std::endl;
        std::cout << "- If you are satisfied with the changes, copy the entire UT2004RPGOUT.txt file to your UT2004RPG.ini file" << std::endl;
        std::cout << "- If you want to re-do the operation, re-run the cleaner which will overwrite the UT2004RPGOUT.txt file." << std::endl << std::endl;
        std::cout << "- If you want to perform another operation while keeping the changes you've just made, rename UT2004RPGOUT.txt to UT2004RPG.txt" << std::endl;
        std::cout << "  The cleaner will then use UT2004RPG.txt as input for a new operation." << std::endl << std::endl;
        
        std::cout << "Enter any key to exit" << std::endl;
        std::cin >> selection;
    }
	return 0;
}

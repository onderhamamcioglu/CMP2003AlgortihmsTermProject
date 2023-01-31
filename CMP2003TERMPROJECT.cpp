// Tested with i5 9300H CPU, 2666 MHZ DDR4 RAM, Kingston M.2 SATA SSD - Acer Nitro 5
// Test 7: 19/01/2021 - Successful - ~30 Seconds.



#include <iostream>
#include <fstream> // used for reading and writing files
#include <chrono> // used for calculating running time
#include <algorithm> // used for converting uppercase letters to lowercase



class customWordMap { // a custom list to hold words as string and their repetition counts as integers
private:
    int capacity;
    std::string* wordArray;
    int currentPosition;
    int* repetitionArray;
public:
    customWordMap(int maxCapacity) {
        currentPosition = 0;
        capacity = maxCapacity;
        wordArray = new std::string[capacity];
        repetitionArray = new int[capacity];
        for (int i = 0; i < capacity; i++) { // to set all repetitions to 0 at the beginnig
            repetitionArray[i] = 0; // could use "repetitionArray[]={0};" but gets stack overflow
        }
    }
    std::string getWord(int position) { // function to get word at given position
        return wordArray[position];
    }
    int getRepetition(int position) { // function to get repetition number of word at given position
        return repetitionArray[position];
    }
    int usedSize() { // returns used size of wordArray
        return currentPosition;
    }
    void add(std::string wordToAdd) { // adds given word to array and increases used size (current position) by 1
        wordArray[currentPosition] = wordToAdd;
        currentPosition++;
    }
    void repeated(int position) { // increases the words repetition count at given position by 1
        repetitionArray[position]++;
    }
    void resetRepetition(int position) { // sets repetition count of word at given position to 0
        repetitionArray[position] = 0;
    }
    ~customWordMap() { // to free memory after our job is done
        delete[] wordArray;
        delete[] repetitionArray;
    }
};



int main() {
    auto beginningTime = std::chrono::high_resolution_clock::now(); // starting timer
    std::ifstream inputFile; // to read files
    std::ofstream outputFile; // to write to files
    std::ofstream logFile; // to keep logs
    logFile.open("logs.txt");
    std::cout << "##### STARTING ##### \n \n" << std::endl;
    logFile << "started" << std::endl;



    // counting lines (words) to create an array for stopwords
    logFile << "counting stopwords" << std::endl;
    int stopwordsCount = 0;
    inputFile.open("stopwords.txt"); // reading stopwords.txt from source folder
    std::string stopwordsFileTemp;
    while (inputFile >> stopwordsFileTemp) { // could use: std::count(std::istreambuf_iterator<char>(inputFile),std::istreambuf_iterator<char>(), '\n')
        stopwordsCount++;                    // but that gives result 1 less than the actual number of lines
    }
    logFile << stopwordsCount << " stopwords found, creating stopwords array" << std::endl;
    std::string* stopwordsArray = new std::string[stopwordsCount]; // dynamically allocating memory for stopwordsArray
    int i = 0;
    inputFile.clear(); // reset inputfile / go back to beginnig
    inputFile.seekg(0);
    logFile << "taking stopwords to array \n";
    while (inputFile >> stopwordsArray[i] && i < stopwordsCount) { // taking stopwordsArray to a queue of strings
        // std::cout << stopwordsArray[i] << std::endl;
        i++;
    }
    inputFile.close();



    //arranging sgm files
    logFile << "arranging sgm files and saving to a new file " << std::endl;
    std::string fileName[22] = { "reut2-000.sgm", "reut2-001.sgm", "reut2-002.sgm", "reut2-003.sgm", "reut2-004.sgm",
                                 "reut2-005.sgm", "reut2-006.sgm", "reut2-007.sgm", "reut2-008.sgm",
                                 "reut2-009.sgm", "reut2-010.sgm", "reut2-011.sgm", "reut2-012.sgm",
                                 "reut2-013.sgm", "reut2-014.sgm", "reut2-015.sgm", "reut2-016.sgm",
                                 "reut2-017.sgm", "reut2-018.sgm", "reut2-019.sgm", "reut2-020.sgm", "reut2-021.sgm" };
    std::string arrangedFilesTo = "arranged.txt";
    outputFile.open(arrangedFilesTo);
    for (int i = 0; i < 22; i++) { // loop for removing some characters and numbers while writing to a new file
        inputFile.open(fileName[i]);
        char tempChar;
        while (!inputFile.eof()) {
            inputFile.get(tempChar);
            if (tempChar == ')' || tempChar == '(' || tempChar == '&' || tempChar == ';' || tempChar == '"' ||
                tempChar == ' ' || /* tempChar == '  '|| */ tempChar == ',' || tempChar == '.' || tempChar == ':' ||
                tempChar == '0' || tempChar == '1' || tempChar == '2' || tempChar == '3' || tempChar == '4' ||
                tempChar == '5' || tempChar == '6' || tempChar == '7' || tempChar == '8' || tempChar == '9' ||
                tempChar == '#' || tempChar == '-') {
                tempChar = '\n';
            }
            outputFile << tempChar;
        }
        inputFile.close();
        logFile << "done: " << fileName[i] << "\n";
    }
    logFile << "finished arranging, written to: " << arrangedFilesTo << std::endl;
    outputFile.close();
    inputFile.open(arrangedFilesTo);
    int wordCount = 0;
    logFile << "counting lines (words) in created file " << std::endl;
    std::string tempString;
    while (inputFile >> tempString) { // counting wordArray in all files
        if (tempString != "\n") {
            wordCount++;
        }
    }
    logFile << "there are total " << wordCount << " words, creating customStack for words" << std::endl;
    customWordMap words(wordCount); // creating a custom stack with max size
    std::string* allWordsArray = new std::string[wordCount]; // keeping all words in an array is faster than reading directly from file



    std::string startString = "</DATELINE><BODY>";
    std::string endString = "</BODY></TEXT>";
    std::cout << "COUNTING WORDS" << std::endl;
    std::cout << "This may take up to a few minutes... \n \n" << std::endl;
    logFile << "started counting words" << std::endl;
    bool isRepeated = false;
    bool isStopword = false;
    bool isBanned = false;
    int used = 0;
    inputFile.clear();
    inputFile.seekg(0);
    while (!inputFile.eof()) { // take all words to array (for better performance)
        inputFile >> allWordsArray[used];
        used++;
    }
    inputFile.close();
    for (int i = 0; i < used; i++) {
        tempString = allWordsArray[i];
        // std::cout << tempString << std::endl;
        if (tempString.substr(0, 17) == startString) { // begin to process wordArray if startString is found
            tempString.erase(0, 17);
            while (tempString != endString) { // keep process until endString
                // std::cout << tempString << std::endl;
                isRepeated = false;
                isStopword = false;
                isBanned = false;
                transform(tempString.begin(), tempString.end(), tempString.begin(), ::tolower); // transform uppercase letters to lowercase letters
                for (int k = 0; k < stopwordsCount; k++) {
                    if (tempString == stopwordsArray[k]) { // if the word is contained in stopwordsArray mark as a stopword
                        isStopword = true;
                        break;
                        // std::cout << "stopword: " << stopwordsArray[k] << std::endl;
                    }
                }
                if (!isStopword) {
                    if (tempString == "/" || tempString =="lt") {
                        isBanned = true;
                    }
                    if (!isBanned) {
                        for (int j = 0; j < words.usedSize(); j++) {
                            if (words.getWord(j)[0] == tempString[0]) { // if first letter of words are different, skip
                                if (words.getWord(j).length() == tempString.length()) { // if length of the words are different, skip
                                    if (words.getWord(j) == tempString) { // if the word is repeated increase repetition count by 1
                                        isRepeated = true;
                                        words.repeated(j);
                                        break;
                                        // std::cout << "repeated: " << tempString << " - repetitions: " << words.getRepetition(j) << std::endl;
                                    }
                                }
                            }
                        }
                        if (!isRepeated) { // if the word is not repeated add to our list
                            words.add(tempString);
                            // std::cout << "unique: " <<tempString << " - position: " << words.usedSize() << std::endl;
                        }
                    }
                }
                i++;
                tempString = allWordsArray[i];
            }
        }
    }
    delete[] stopwordsArray; // free memory used by stopwords
    delete[] allWordsArray;
    logFile << "finished counting" << std::endl;



    int indexOfLargest = 0;
    logFile << "TOP10 REPEATED WORDS ARE:" << std::endl;
    std::cout << "PRINTING TOP 10 REPEATED WORDS \n" << std::endl;
    for (int j = 0; j < 10; j++) { // loop to print top 10 repeated words
        for (int i = 0; i < words.usedSize(); i++) {
            if (words.getRepetition(i) > words.getRepetition(indexOfLargest)) {
                indexOfLargest = i;
            }
        }
        std::cout << words.getWord(indexOfLargest) << " - " << words.getRepetition(indexOfLargest) << std::endl;
        logFile << words.getWord(indexOfLargest) << " - " << words.getRepetition(indexOfLargest) << std::endl;
        words.resetRepetition(indexOfLargest);
    }
    std::cout << std::endl;



    auto endingTime = std::chrono::high_resolution_clock::now(); // stopping timer
    std::chrono::duration<double> elapsedTime = endingTime - beginningTime; // calculate elapsed time
    std::cout << "Total Elapsed Time: " << (int)(elapsedTime.count() / 60) << " minutes " << (int)elapsedTime.count() % 60 << " seconds \n \n" << std::endl;
    std::cout << "Check logs.txt for details" << std::endl;
    logFile << "total elapsed time: " << (int)(elapsedTime.count() / 60) << " minutes " << (int)elapsedTime.count() % 60 << " seconds" << std::endl;
    logFile.close();
    return 0;
}

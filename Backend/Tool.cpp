#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <vector>
#include "imgui.h"
namespace Tool {
    std::vector<std::vector<unsigned char>> HEXA(unsigned char buffer[], int bufferSize) {
        std::vector<unsigned char> tmp;
        for (int i = 0; i < bufferSize; i++) {
            char byte = buffer[i];
            std::bitset<4> high_nibble(byte >> 4);
            std::bitset<4> low_nibble(byte & 0x0F);
            if (high_nibble.to_string() == "0000") {
                tmp.push_back('0');
            }
            if (high_nibble.to_string() == "0001") {
                tmp.push_back('1');
            }
            if (high_nibble.to_string() == "0010") {
                tmp.push_back('2');
            }
            if (high_nibble.to_string() == "0011") {
                tmp.push_back('3');
            }
            if (high_nibble.to_string() == "0100") {
                tmp.push_back('4');
            }
            if (high_nibble.to_string() == "0101") {
                tmp.push_back('5');
            }
            if (high_nibble.to_string() == "0110") {
                tmp.push_back('6');
            }
            if (high_nibble.to_string() == "0111") {
                tmp.push_back('7');
            }
            if (high_nibble.to_string() == "1000") {
                tmp.push_back('8');
            }
            if (high_nibble.to_string() == "1001") {
                tmp.push_back('9');
            }
            if (high_nibble.to_string() == "1010") {
                tmp.push_back('A');
            }
            if (high_nibble.to_string() == "1011") {
                tmp.push_back('B');
            }
            if (high_nibble.to_string() == "1100") {
                tmp.push_back('C');
            }
            if (high_nibble.to_string() == "1101") {
                tmp.push_back('D');
            }
            if (high_nibble.to_string() == "1110") {
                tmp.push_back('E');
            }
            if (high_nibble.to_string() == "1111") {
                tmp.push_back('F');
            }

            if (low_nibble.to_string() == "0000") {
                tmp.push_back('0');
            }
            if (low_nibble.to_string() == "0001") {
                tmp.push_back('1');
            }
            if (low_nibble.to_string() == "0010") {
                tmp.push_back('2');
            }
            if (low_nibble.to_string() == "0011") {
                tmp.push_back('3');
            }
            if (low_nibble.to_string() == "0100") {
                tmp.push_back('4');
            }
            if (low_nibble.to_string() == "0101") {
                tmp.push_back('5');
            }
            if (low_nibble.to_string() == "0110") {
                tmp.push_back('6');
            }
            if (low_nibble.to_string() == "0111") {
                tmp.push_back('7');
            }
            if (low_nibble.to_string() == "1000") {
                tmp.push_back('8');
            }
            if (low_nibble.to_string() == "1001") {
                tmp.push_back('9');
            }
            if (low_nibble.to_string() == "1010") {
                tmp.push_back('A');
            }
            if (low_nibble.to_string() == "1011") {
                tmp.push_back('B');
            }
            if (low_nibble.to_string() == "1100") {
                tmp.push_back('C');
            }
            if (low_nibble.to_string() == "1101") {
                tmp.push_back('D');
            }
            if (low_nibble.to_string() == "1110") {
                tmp.push_back('E');
            }
            if (low_nibble.to_string() == "1111") {
                tmp.push_back('F');
            }
        }

        // Array size
        size_t size = tmp.size();
        size_t numColumns = 32;
        size_t numRows = (size + numColumns - 1) / numColumns;
        std::vector<std::vector<unsigned char>> hexArray(numRows, std::vector<unsigned char>(numColumns, 0));

        for (size_t i = 0; i < size; i++) {
            size_t row = i / numColumns;
            size_t col = i % numColumns;
            hexArray[row][col] = tmp[i];
        }
        return hexArray;
    }
    std::vector<std::vector<unsigned char>> InitializeFileData() {
        std::vector<std::vector<unsigned char>> filedata;
        // Sample data (you can replace this with your actual data)
        const char* sampleData[] = {
            "0123456789ABCDEF",
            "FEDCBA9876543210",
            "A1B2C3D4E5F67890",
            "1F2E3D4C5B6A7980"
            // Add more rows as needed
        };

        // Convert the sample data to unsigned char and store in the 2D vector
        for (const char* row : sampleData) {
            std::vector<unsigned char> rowData(row, row + strlen(row));
            filedata.push_back(rowData);
        }

        return filedata;
    }
}
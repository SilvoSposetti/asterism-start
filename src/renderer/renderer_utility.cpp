#include "renderer/renderer_utility.h"

#include <utility>

void VK_CHECK(VkResult result) {
    VK_CHECK(result, "Unknown");
}

void VK_CHECK(VkResult result, const char *message) {
    if (result != VK_SUCCESS) {
        char error_msg[30] = "Something went wrong during: ";
        throw std::runtime_error(strcat(error_msg, message));
    }
}


// Prints standard text
void print(const char *message) {
    std::cout << message << std::endl;
}

void print(const std::string& message) {
    std::cout << message << std::endl;
}

// Prints centered text
void log(const std::string& message) {
    int lineLength = 80;

    int messageLength = message.length();
    int linesNeeded = (int) messageLength / lineLength + 1;

    for (int i = 0; i < linesNeeded; i++) {
        std::string paddedText;
        if (i == (linesNeeded - 1)) {
            int remaining = messageLength - i * lineLength;
            paddedText = padText(message.substr(i * lineLength, remaining), lineLength);
        } else {
            paddedText = padText(message.substr(i * lineLength, lineLength), lineLength);
        }
        print(paddedText);
    }
}

void log(const char *message) {
    // Convert message to std::string
    std::string text = std::string(message);
    log(std::string(text));
}

void logTitle(std::string message) {
    std::string newText = std::move(message);
    int n = 10;
    newText.insert(newText.begin(), 1, ' ');
    newText.insert(newText.begin(), 1, '[');
    newText.insert(newText.begin(), n, '=');
    newText.insert(newText.end(), 1, ' ');
    newText.insert(newText.end(), 1, ']');
    newText.insert(newText.end(), n, '=');
    log(newText);
}

void logTitle(const char *message) {
    std::string text = std::string(message);
    logTitle(std::string(text));
}


std::string padText(const std::string& text, int maxLineLength) {
    std::string newText = text;
    int n = (maxLineLength - (int) text.length()) / 2;
    newText.insert(newText.begin(), n, ' ');
    return newText;
}
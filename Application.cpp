#include "Application.h"
#include "Logger.h"
#include "Command.h"
#include "classes/TicTacToe.h"
#include "imgui/imgui.h"
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

namespace ClassGame {
    
    // Global TicTacToe variables
    static TicTacToe *game = nullptr;
    static bool gameOver = false;
    static int gameWinner = -1;
    
    // "Game Control" Window defaults
    static int gameActCounter = 0;                              // Track player actions by count
    static float floatVal = 0.0f;                               // Displays designated float val on slider
    static ImVec4 clearColor = ImVec4(0.5f, 0.5f, 0.5f, 1.00f); // Hex picker default

    static bool DemoWin = false;     // Disable demo window by default
    static bool LogWin = true;
    static bool GameWin = true;      // TicTacToe game window
    static bool ControlWin = true;   // Game control panel

    // Default hex color (clear)
    static float colorR = 115.0f / 255.0f;
    static float colorG = 140.0f / 255.0f;
    static float colorB = 153.0f / 255.0f;
    
    // Command line input buffer and history
    static char InputBuf[256] = "";
    
    void ResetGameCounter() {
        gameActCounter = 0;
    }

    void ResetGame() {
        if (!game) return;
        game->stopGame();
        game->setUpBoard();
        gameOver = false;
        gameWinner = -1;
        LOG_INFO("Game reset - new game started");
    }

    void GameStartUp() {
        // Initialize Logger
        Logger::GetInstance().Init();

        // Initialize TicTacToe game
        game = new TicTacToe();
        game->setUpBoard();
        
        // Test log entry types/tags
        LOG_INFO("TicTacToe game initialized");
        LOG_INFO_TAG("Player 1: X | Player 2: O", "GAME");
        
        // Initialize control variables
        gameActCounter = 0;
        floatVal = 0.0f;
        DemoWin = false;
        LogWin = true;
        GameWin = true;
        ControlWin = true;
        gameOver = false;
        gameWinner = -1;
    }

    void RenderGame() {
        
        // Create dock space
        ImGui::DockSpaceOverViewport();
        
        // Optional demo window
        if (DemoWin) { 
            ImGui::ShowDemoWindow(&DemoWin);
        }

        // Window #1 - TicTacToe Game Window
        if (GameWin && game) {
            ImGui::Begin("TicTacToe Game", &GameWin, ImGuiWindowFlags_NoScrollbar);
            
            // Display current player and game state
            if (game->getCurrentPlayer()) {
                ImGui::Text("Current Player: %s (Player %d)", 
                    game->getCurrentPlayer()->playerNumber() == 0 ? "X" : "O", 
                    game->getCurrentPlayer()->playerNumber() + 1);
            }
            
            // Game status
            if (gameOver) {
                if (gameWinner == -1) {
                    ImGui::Text("Game Over: It's a Draw!");
                } else {
                    ImGui::Text("Game Over! Winner: Player %d", gameWinner);
                }
                
                if (ImGui::Button("Play Again")) {
                    ResetGame();
                }
                ImGui::SameLine();
            }
            
            if (!gameOver && ImGui::Button("Reset Game")) {
                ResetGame();
            }
            
            ImGui::Separator();
            
            // Draw the game board
            game->drawFrame();
            
            ImGui::End();
        }

        // Window #2 - Game Log with Command Line
        if (LogWin) {
            ImGui::Begin("Game Log", &LogWin);

            // Filter state variables
            static bool showInfo = true;
            static bool showWarning = true;
            static bool showError = true;

            // Options button and popup
            if (ImGui::Button("Options")) {
                ImGui::OpenPopup("OptionsPopup");
            }

            if (ImGui::BeginPopup("OptionsPopup")) {
                ImGui::Text("Filter Options");
                ImGui::Separator();
                
                ImGui::Checkbox("Show Info", &showInfo);
                ImGui::Checkbox("Show Warnings", &showWarning);
                ImGui::Checkbox("Show Errors", &showError);
                
                ImGui::Separator();
                
                if (ImGui::MenuItem("Clear Log")) {
                    Logger::GetInstance().Clear();
                }
                
                ImGui::EndPopup();
            }

            // Test buttons
            ImGui::SameLine();
            if (ImGui::Button("Clear")) {
                Logger::GetInstance().Clear();
            }
            ImGui::SameLine();
            if (ImGui::Button("Test Info")) {
                LOG_INFO("This is a test info message");
            }
            ImGui::SameLine();
            if (ImGui::Button("Test Warning")) {
                LOG_WARN("This is a test warning message");
            }
            ImGui::SameLine();
            if (ImGui::Button("Test Error")) {
                LOG_ERROR("This is a test error message");
            }
            ImGui::Separator();

            // Display log entries with filtering
            const auto& entries = Logger::GetInstance().GetEntries();
            const auto& colors = Logger::GetInstance().GetColors();
            
            const float footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
            ImGui::BeginChild("LogScrollRegion", ImVec2(0, -footer_height), true);
            
            for (size_t i = 0; i < entries.size(); i++) {
                bool display = true;
                
                if (entries[i].find("[INFO]") != std::string::npos && !showInfo) { 
                    display = false; 
                }
                else if (entries[i].find("[WARN]") != std::string::npos && !showWarning) { 
                    display = false; 
                }
                else if (entries[i].find("[ERROR]") != std::string::npos && !showError) { 
                    display = false; 
                }
                
                if (display) {
                    ImGui::PushStyleColor(ImGuiCol_Text, colors[i]);
                    ImGui::Text("%s", entries[i].c_str());
                    ImGui::PopStyleColor();
                }
            }
            
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();

            // Command line input
            ImGui::Separator();
            bool reclaim_focus = false;

            ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | 
                                                   ImGuiInputTextFlags_EscapeClearsAll | 
                                                   ImGuiInputTextFlags_CallbackHistory;
            
            if (ImGui::InputText("##CommandInput", InputBuf, IM_ARRAYSIZE(InputBuf), 
                                input_text_flags, &Command::TextEditCallbackStub)) {
                char* s = InputBuf;
                Command::Strtrim(s);
                if (s[0])
                    Command::ExecCommand(s);
                strcpy_s(s, IM_ARRAYSIZE(InputBuf), "");
                reclaim_focus = true;
            }
            
            ImGui::SetItemDefaultFocus();
            if (reclaim_focus)
                ImGui::SetKeyboardFocusHere(-1);

            ImGui::SameLine();
            ImGui::Text("Command");

            ImGui::SameLine();
            if (ImGui::Button("Help")) {
                LOG_INFO_TAG("Available commands: CLEAR, HELP, INFO, WARN, ERROR, RESET", "CMD");
            }

            ImGui::End();
        }

        // Window #3 - Game Control Panel
        if (ControlWin) {
            ImGui::Begin("Game Control", &ControlWin);
            ImGui::Text("Main Game Control Panel");

            // Window toggles
            ImGui::Checkbox("##DemoCheck", &DemoWin);
            ImGui::SameLine();
            ImGui::Text("Demo Window");

            ImGui::Checkbox("##LogCheck", &LogWin);
            ImGui::SameLine();
            ImGui::Text("Log Window");

            ImGui::Checkbox("##GameCheck", &GameWin);
            ImGui::SameLine();
            ImGui::Text("Game Window");

            ImGui::Checkbox("##ControlCheck", &ControlWin);
            ImGui::SameLine();
            ImGui::Text("This Window");

            ImGui::Separator();

            // Game control buttons
            ImGui::Text("TicTacToe Controls:");
            
            if (ImGui::Button("Reset TicTacToe")) {
                if (game) {
                    game->stopGame();
                    game->setUpBoard();
                    gameOver = false;
                    gameWinner = -1;
                    LOG_INFO("Game reset from control panel");
                }
            }

            // Game status display
            ImGui::Separator();
            ImGui::Text("Game Status:");
            if (game) {
                ImGui::Text("Current Player: %s", 
                    game->getCurrentPlayer() ? 
                    (game->getCurrentPlayer()->playerNumber() == 1 ? "X" : "O") : "None");
                ImGui::Text("Game Over: %s", gameOver ? "Yes" : "No");
                if (gameOver) {
                    ImGui::Text("Winner: %s", 
                        gameWinner == -1 ? "Draw" : 
                        (gameWinner == 1 ? "Player 1 (X)" : "Player 2 (O)"));
                }
            }

            ImGui::Separator();
            // Visual controls
            ImGui::SliderFloat("##float", &floatVal, 0.0f, 1.0f, "%.3f");
            ImGui::SameLine();
            ImGui::Text("Test Slider");

            // Color edit control
            float colorArray[3] = { colorR, colorG, colorB };
            if (ImGui::ColorEdit3("Background Color", colorArray)) {
                colorR = colorArray[0];
                colorG = colorArray[1];
                colorB = colorArray[2];
                clearColor = ImVec4(colorR, colorG, colorB, 1.0f);
            }

            // Action counter
            if (ImGui::Button("Game Action")) {
                gameActCounter++;
                std::string msg = "Game action performed, counter: " + std::to_string(gameActCounter);
                LOG_INFO_TAG(msg, "GAME");
            }
            ImGui::SameLine();
            ImGui::Text("Action Counter: %d", gameActCounter);

            ImGui::Separator();
            ImGui::Text("Logging Test Buttons:");

            if (ImGui::Button("Log Game Event")) {
                LOG_INFO_TAG("Manual game event from control panel", "GAME");
            }
            ImGui::SameLine();
            if (ImGui::Button("Log Warnings")) {
                LOG_WARN("Manual warning from control panel");
            }
            ImGui::SameLine();
            if (ImGui::Button("Log Error")) {
                LOG_ERROR("Manual error from control panel");
            }

            ImGui::End();
        }
    }

    void EndOfTurn() {
        if (!game) return;
        
        // Check for winner or draw
        Player *winner = game->checkForWinner();
        if (winner) {
            gameOver = true;
            gameWinner = winner->playerNumber() + 1;
            LOG_INFO_TAG("Game Over! Winner: Player " + std::to_string(gameWinner), "GAME");
        } else if (game->checkForDraw()) {
            gameOver = true;
            gameWinner = -1;
            LOG_INFO_TAG("Game Over! It's a draw.", "GAME");
        }
        
        // Increment action counter and log
        gameActCounter++;
        if (!gameOver) {
            // Log the state string after the move
            // Note: getCurrentPlayer() has already switched, so we need the previous player
            int previousPlayerNum = (game->getCurrentPlayer()->playerNumber() + 1) % 2 + 1;
            std::string state = game->stateString();
            
            // If it was player 2's (AI) turn, log detailed evaluations
            if (previousPlayerNum == 2) {
                auto evaluations = game->getLastAIEvaluations();
                int choice = game->getLastAIChoice();
                
                // Log each position's evaluation
                for (const auto& eval : evaluations) {
                    int pos = eval.first;
                    int score = eval.second;
                    std::string chosenStr = (pos == choice) ? " <- CHOSEN" : "";
                    LOG_INFO_TAG("  Position " + std::to_string(pos) + " (row " + std::to_string(pos/3) + 
                                ", col " + std::to_string(pos%3) + "): score = " + std::to_string(score) + chosenStr, "AI");
                }
            }
            
            LOG_INFO_TAG("End of turn #" + std::to_string(gameActCounter) + 
                        " | Player: " + std::to_string(previousPlayerNum) +
                        " | Board State: " + state, 
                        "GAME");
        }
    }
}
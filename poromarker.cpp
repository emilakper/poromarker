﻿#pragma once

#include <iostream>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include<imgui_impl_opengl3_loader.h>
#include<imgui_internal.h>
#include<imgui_stdlib.h>
#include<GLFW/glfw3.h>
#include<GLFW/glfw3native.h>
#include<opencv2/opencv.hpp>
#include<imfilebrowser/imfilebrowser.h>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <ImageProcessor/ImageProcessor.hpp>
#include <ObjData/ObjData.hpp>
#include <loader/loader.hpp>

#ifdef _WIN32
#include <Windows.h>
#include <shellapi.h>

void OpenURLInBrowser(const std::string& url) {
    ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}
#elif __APPLE__
#include <cstdlib>

void OpenURLInBrowser(const std::string& url) {
    std::string command = "open " + url;
    system(command.c_str());
}
#elif __linux__
#include <cstdlib>

void OpenURLInBrowser(const std::string& url) {
    std::string command = "xdg-open " + url;
    system(command.c_str());
}
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505)
#endif

#include <math.h> 
#include <time.h> 
#include <implot.h>
#include <imgui_impl_glut.h>
#include <imgui_impl_opengl2.h>
#include <opencv2/core/mat.hpp>
#include<ObjData/ObjData.hpp>

#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

void MainLoopStep();
void GetData();
void ProcessedWindow();
void ChooseWindow();
bool CheckSubvolume();
void SubvolumeErrorWindow();
void SolveMarkupErrors(std::vector <cv::Mat>);
void TooManyErrorsWindow();

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

GLuint convertMatToTexture(const cv::Mat& image) {
    cv::Mat imageRGB;
    cv::cvtColor(image, imageRGB, cv::COLOR_BGR2RGB);
    GLuint imageTexture;
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 0x1907, imageRGB.size().width, imageRGB.size().height, 0, 0x1907, GL_UNSIGNED_BYTE, imageRGB.data);
    return imageTexture;
}

void checkLimits(int& num, int min, int max) {
    if (num < min) {
        num = min;
    }
    else if (num > max) {
        num = max;
    }
}

cv::Mat modifyColors(const cv::Mat& image) {
    cv::Mat modifiedImage = image.clone();
    cv::cvtColor(modifiedImage, modifiedImage, cv::COLOR_GRAY2BGR);
    for (int y = 0; y < modifiedImage.rows; ++y) {
        cv::Vec3b* row = modifiedImage.ptr<cv::Vec3b>(y);
        for (int x = 0; x < modifiedImage.cols; ++x) {
            cv::Vec3b& color = row[x];
            if (color == cv::Vec3b(255, 255, 255)) {
                color = cv::Vec3b(128, 0, 128);
            }
            else if (color == cv::Vec3b(0, 0, 0)) {
                color = cv::Vec3b(0, 255, 255);
            }
        }
    }

    return modifiedImage;
}

ObjData poroInfo;
static bool show_processed_window = false;
static bool show_plot_x = true;
static bool show_plot_y = false;
static bool show_plot_z = false;
static bool show_choose_window = false;
static bool show_sub_volume = false;
static bool show_subvolume_error_window = false;
std::vector<double> x_axis, y_axis, z_axis;
static float dir_x[3]{ 1, 0, 0 };
static float dir_y[3]{ 0, 1, 0 };
static float dir_z[3]{ 0, 0, 1 };
static double absolute_porosity = 0.0;
static double relative_porosity = 0.0;
static double pixel_volume = 0.0;
static double sub_absolute_porosity = 0.0;
static double sub_relative_porosity = 0.0;
std::vector<int> sizes{ 0, 0, 0 };
static int xs[2]{ 1, 1 };
static int ys[2]{ 1, 1 };
static int zs[2]{ 1, 1 };
static int subvolume[6]{ 1, 1, 1, 1, 1, 1 };

static bool markup_error = false;
static bool show_too_many_errors_window = false;
std::vector<cv::Mat> mats{};


void GetData() {
    sizes = poroInfo.GetSizes();
    xs[1] = sizes[0];
    ys[1] = sizes[1];
    zs[1] = sizes[2];
    subvolume[1] = sizes[0];
    subvolume[3] = sizes[1];
    subvolume[5] = sizes[2];

    absolute_porosity = poroInfo.GetAbsolutePorosity();
    relative_porosity = poroInfo.GetRelativePorosity();
    pixel_volume = poroInfo.GetPixelVolume();

    x_axis = poroInfo.GetPorosityDistribution(dir_x);
    x_axis.insert(x_axis.begin(), 0);
    y_axis = poroInfo.GetPorosityDistribution(dir_y);
    y_axis.insert(y_axis.begin(), 0);
    z_axis = poroInfo.GetPorosityDistribution(dir_z);
    y_axis.insert(y_axis.begin(), 0);
}

void ProcessedWindow() {

    ImVec2 Window_pos = { 180, 140 };
    ImVec2 Window_size = { 920, 560 };
    ImVec2 Button_size = { 100, 30 };

    ImGui::SetNextWindowPos(Window_pos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(Window_size, ImGuiCond_Once);

    ImGui::OpenPopup("Processed");

    if (ImGui::BeginPopupModal("Processed", nullptr, 0)) {

        if (ImGui::Button("XPlot", Button_size)) {
            show_plot_y = false;
            show_plot_z = false;
            show_plot_x = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("YPlot", Button_size)) {
            show_plot_x = false;
            show_plot_z = false;
            show_plot_y = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("ZPlot", Button_size)) {
            show_plot_x = false;
            show_plot_y = false;
            show_plot_z = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("SubVolume", Button_size)) {
            show_choose_window = true;
        }

        if (show_plot_x) {
            if (ImPlot::BeginPlot("XPlot")) {
                ImPlot::SetupAxisLimits(ImAxis_X1, 1, sizes[0], ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, sizes[1] * sizes[2] * pixel_volume, ImPlotCond_Always);
                ImPlot::PlotBars("X_slice", x_axis.data(), x_axis.size(), 1);
                ImPlot::EndPlot();
            }
        }

        if (show_plot_y) {
            if (ImPlot::BeginPlot("YPlot")) {
                ImPlot::SetupAxisLimits(ImAxis_X1, 1, sizes[1], ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, sizes[0] * sizes[2] * pixel_volume, ImPlotCond_Always);
                ImPlot::PlotBars("Y_slice", y_axis.data(), y_axis.size(), 1);
                ImPlot::EndPlot();
            }
        }

        if (show_plot_z) {
            if (ImPlot::BeginPlot("ZPlot")) {
                ImPlot::SetupAxisLimits(ImAxis_X1, 1, sizes[2], ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, sizes[0] * sizes[1] * pixel_volume, ImPlotCond_Always);
                ImPlot::PlotBars("Z_slice", z_axis.data(), z_axis.size(), 1);
                ImPlot::EndPlot();
            }
        }

        ImGui::Text("Pore volume : %.3f", absolute_porosity);
        ImGui::Text("Relative porosity : %.6f", relative_porosity);

        if (show_sub_volume) {
            ImGui::NewLine();
            ImGui::Text("Subvolume metrics");
            ImGui::Text("Pore volume : %.3f", sub_absolute_porosity);
            ImGui::Text("Relative porosity : %.6f", sub_relative_porosity);
        }

        if (ImGui::Button("Close")) {
            show_processed_window = false;
            ImGui::CloseCurrentPopup();
        }

        if (show_choose_window)
        {
            ChooseWindow();
        }

        ImGui::EndPopup();
    }
}

void ChooseWindow() {
    ImVec2 Window_pos = { 200, 210 };
    ImVec2 Window_size = { 300, 250 };

    ImGui::SetNextWindowPos(Window_pos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(Window_size, ImGuiCond_Once);

    ImGui::OpenPopup("Choose subvolume");

    if (ImGui::BeginPopupModal("Choose subvolume", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter subvolume");

        ImGui::Text("X range:");
        ImGui::SameLine();
        ImGui::InputInt2("  ", xs);
        ImGui::Text("Available X range: [%d; %d]", 1, sizes[0]);
        ImGui::NewLine();

        ImGui::Text("Y range:");
        ImGui::SameLine();
        ImGui::InputInt2("   ", ys);
        ImGui::Text("Available Y range: [%d; %d]", 1, sizes[1]);
        ImGui::NewLine();

        ImGui::Text("Z range:");
        ImGui::SameLine();
        ImGui::InputInt2("    ", zs);
        ImGui::Text("Available Z range: [%d; %d]", 1, sizes[2]);
        ImGui::NewLine();

        if (ImGui::Button("Enter")) {
            if (!CheckSubvolume()) {
                show_subvolume_error_window = true;
            }
            else {
                subvolume[0] = xs[0];
                subvolume[1] = xs[1];
                subvolume[2] = ys[0];
                subvolume[3] = ys[1];
                subvolume[4] = zs[0];
                subvolume[5] = zs[1];
                std::pair<double, double> sub_data = poroInfo.GetPartialPorosity(subvolume);
                sub_absolute_porosity = sub_data.first;
                sub_relative_porosity = sub_data.second;
                show_choose_window = false;
                show_sub_volume = true;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset")) {
            xs[0] = 1;
            xs[1] = sizes[0];
            ys[0] = 1;
            ys[1] = sizes[1];
            zs[0] = 1;
            zs[1] = sizes[2];
        }

        if (show_subvolume_error_window) {
            SubvolumeErrorWindow();
        }

        ImGui::EndPopup();
    }
}

bool CheckSubvolume() {
    if (1 <= xs[0] && xs[0] <= xs[1] && xs[1] <= sizes[0] &&
        1 <= ys[0] && ys[0] <= ys[1] && ys[1] <= sizes[1] &&
        1 <= zs[0] && zs[0] <= zs[1] && zs[1] <= sizes[2]) {
        return true;
    }
    else {
        return false;
    }
}

void SubvolumeErrorWindow() {
    ImVec2 Window_pos = { 495, 280 };
    ImVec2 Window_size = { 290, 80 };

    ImGui::SetNextWindowPos(Window_pos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(Window_size, ImGuiCond_Once);

    ImGui::OpenPopup("Error");

    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Please enter valid subvolume bounds");
        if (ImGui::Button("Close")) {
            show_subvolume_error_window = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void SolveMarkupErrors(std::vector<cv::Mat> mats) {
    ImVec2 Window_pos = { 495, 280 };
    ImVec2 Window_size = { 290, 80 };

    ImGui::SetNextWindowPos(Window_pos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(Window_size, ImGuiCond_Once);

    ImGui::OpenPopup("Markup error");

    if (ImGui::BeginPopupModal("Markup error", nullptr, 2)) {
        ImGui::Text("Markup error occurred on %d slices", mats.size());

        if (ImGui::Button("Ignore")) {
            markup_error = false;
            GetData();
            show_processed_window = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Change filter")) {
            markup_error = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Manual marking")) {
            if (mats.size() > 10) {
                show_too_many_errors_window = true;
            }
            else {
                markup_error = false;
                ImGui::CloseCurrentPopup();
            }
        }

        if (show_too_many_errors_window) {
            TooManyErrorsWindow();
        }

        ImGui::EndPopup();
    }
}

void TooManyErrorsWindow() {
    ImVec2 Window_pos = { 465, 280 };
    ImVec2 Window_size = { 350, 90 };

    ImGui::SetNextWindowPos(Window_pos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(Window_size, ImGuiCond_Once);

    ImGui::OpenPopup("Too many errors");

    if (ImGui::BeginPopupModal("Too many errors", nullptr, 2)) {
        ImGui::Text("There are too many markup errors.");
        ImGui::Text("We highly recommend not to use manual marking.");

        if (ImGui::Button("Still manual marking")) {
            show_too_many_errors_window = false;
            markup_error = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Back")) {
            show_too_many_errors_window = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    GLFWwindow* window = glfwCreateWindow(1920, 1000, "PoroMarker", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontGlobalScale = 1.5f;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Variables and Constructor Calls
    bool show_start_window = true; // Startup Window
    bool show_project_window = false; // Project Window
    bool showErrorPopup = false;
    bool showExitPopup = false;
    bool showErrorOpenPopup = false;
    bool showDummyWindow = false;
    std::string url = "https://github.com/emilakper/poromarker";

    ve::DirectoryLoader dir_loader;

    ImGui::FileBrowser dirDialog(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_MultipleSelection |
        ImGuiFileBrowserFlags_MultipleSelection | ImGuiFileBrowserFlags_CloseOnEsc |
        ImGuiFileBrowserFlags_CreateNewDir);
    dirDialog.SetTitle("Choose folder");
    ImGui::FileBrowser fileDialog(ImGuiFileBrowserFlags_MultipleSelection | ImGuiFileBrowserFlags_CloseOnEsc);
    fileDialog.SetTitle("Choose files");
    fileDialog.SetTypeFilters({ ".png",".tif" });
    ImGui::FileBrowser projDirDialog(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc |
        ImGuiFileBrowserFlags_CreateNewDir);
    projDirDialog.SetTitle("Choose Project Folder");
    ImGui::FileBrowser projOpenDirDialog(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc);
    projOpenDirDialog.SetTitle("Choose Project Folder");

    std::string picsPath = std::filesystem::path(__FILE__).parent_path().string() + "/pics/";
#ifdef _WIN32
    std::replace(picsPath.begin(), picsPath.end(), '/', '\\');
#endif
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    cv::Mat image = cv::imread(picsPath + "logo.png", cv::IMREAD_COLOR);
    GLuint imageTexture = convertMatToTexture(image);
    cv::Mat tempImage;

    std::vector<cv::Mat> layerImages;
    std::vector<cv::Mat> masksImages;
    int layerNumber = 0;
    bool maskOn = false;

    layerImages.push_back(cv::imread(picsPath + "nolayerpic.png", cv::IMREAD_COLOR));
    masksImages.push_back(cv::imread(picsPath + "maskexample.png", cv::IMREAD_GRAYSCALE));
    auto itr = layerImages.begin();
    int itrEnd = layerImages.size() - 1;
    auto itrm = masksImages.begin();
    int itrmEnd = masksImages.size() - 1;
    GLuint imageLayerTexture = convertMatToTexture(*itr);
    GLuint maskLayerTexture = convertMatToTexture(modifyColors(*itrm));

    float oriTrans = 0.55f;
    float maskTrans = 0.9f;

    const char* filters[] = { "BILATERAL", "NLM", "None" };
    int currentFilter = 2;
    int bdc = 0;
    int filterIterations = 2;
    int filterhParam = 10;
    int ksize = 5;
    int threshold = 128;
    const char* thresholds[] = { "BINARY", "OTSU", "MEAN_STD_DEV", "KAPUR" };
    int currentThreshold = 0;

    bool mode = false;
    std::string errorMessage;

    // cv::Mat parampic = cv::imread(picsPath + "parampic.png", cv::IMREAD_COLOR);
    GLuint paramTexture = convertMatToTexture(cv::imread(picsPath + "parampic.png", cv::IMREAD_COLOR));
    GLuint paramResult = convertMatToTexture(cv::imread(picsPath + "parampic.png", cv::IMREAD_COLOR));
    std::vector<cv::Mat> paramPics;
    std::vector<cv::Mat> paramRes;
    paramPics.push_back(cv::imread(picsPath + "parampic.png", cv::IMREAD_COLOR));
    ImageProcessor segment;
    ImageProcessor::Settings config;

    const int cursorWidth = 16;
    const int cursorHeight = 16;

    // Создание текстурных данных для крестика
    unsigned char cursorData[cursorWidth * cursorHeight * 4];
    for (int y = 0; y < cursorHeight; ++y) {
        for (int x = 0; x < cursorWidth; ++x) {
            bool isDiagonalPixel = (x == y || x == (cursorWidth - y - 1));
            int offset = (x + y * cursorWidth) * 4;
            cursorData[offset] = isDiagonalPixel ? 255 : 0; // Красный канал
            cursorData[offset + 1] = 0; // Зеленый канал
            cursorData[offset + 2] = 0; // Синий канал
            cursorData[offset + 3] = isDiagonalPixel ? 255 : 0; // Альфа-канал (непрозрачный)
        }
    }

    // Создание кастомного курсора из текстурных данных
    GLFWimage cursimage;
    cursimage.width = cursorWidth;
    cursimage.height = cursorHeight;
    cursimage.pixels = cursorData;
    GLFWcursor* customCursor = glfwCreateCursor(&cursimage, cursorWidth / 2, cursorHeight / 2);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (show_start_window)
        {
            ImVec2 size(230, 375);
            ImGui::SetNextWindowSize(size, ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - size.x) * 0.5f, (ImGui::GetIO().DisplaySize.y - size.y) * 0.5f));
            ImGui::Begin("Welcome to PoroMarker", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::SetWindowFontScale(1.0f * (ImGui::GetIO().DisplaySize.y / 1080));
            ImGui::Text("Choose an option.");
            ImGui::NewLine();
            if (ImGui::Button("Create project")) {
                projDirDialog.Open();
            }
            if (ImGui::Button("Open project")) {
                projOpenDirDialog.Open();
            }
            if (ImGui::Button("Help")) {
                OpenURLInBrowser(url);
            }
            ImGui::Image((void*)(intptr_t)imageTexture, ImVec2(image.size().width, image.size().height));
            ImGui::End();
        }

        if (show_project_window) {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            ImVec2 size(static_cast<float>(width), static_cast<float>(height));
            ImGui::SetNextWindowSize(size);
            ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - size.x) * 0.5f, (ImGui::GetIO().DisplaySize.y - size.y)));
            ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
                | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Project")) {
                    if (ImGui::MenuItem("Open folder")) {
                        dirDialog.Open();
                    }
                    if (ImGui::MenuItem("Open files")) {
                        fileDialog.Open();
                    }
                    if (ImGui::MenuItem("Save")) {
                        // Saving function
                        segment.updateConfig(config);
                    }
                    if (ImGui::MenuItem("Exit")) {
                        showExitPopup = true;
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help")) {
                    if (ImGui::MenuItem("Short User Guide")) {
                        OpenURLInBrowser(url);
                    }
                    ImGui::EndMenu();
                }

                // Instruments Buttons
                ImGui::Separator();

                if (ImGui::Button("Scissors", ImVec2(160 * width / 1920, 0))) {
                    showDummyWindow = true;
                }
                ImGui::SameLine();

                if (ImGui::Button("Rectangle", ImVec2(160 * width / 1920, 0))) {
                    showDummyWindow = true;
                }

                if (ImGui::Button("Erase", ImVec2(160 * width / 1920, 0))) {
                    showDummyWindow = true;
                }

                if (ImGui::Button("PolyLine", ImVec2(160 * width / 1920, 0))) {
                    showDummyWindow = true;
                }
                ImGui::EndMenuBar();
            }

            ImGui::Columns(2, "myColumns", false);

            float columnWidth1 = ImGui::GetWindowSize().x * 0.625f;
            float columnWidth2 = ImGui::GetWindowSize().x * 0.375f;

            ImGui::SetColumnWidth(0, columnWidth1);
            ImGui::SetColumnWidth(1, columnWidth2);

            ImDrawList* drawList = ImGui::GetWindowDrawList();

            if (!maskOn) {
                ImGui::Image((void*)(intptr_t)imageLayerTexture, ImVec2(height - 100, height - 100));
            }
            else {
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + height - 100, p0.y + height - 100);
                ImVec4 tint = ImVec4(1.0f, 1.0f, 1.0f, oriTrans);
                drawList->AddImage((void*)(intptr_t)imageLayerTexture, p0, p1, ImVec2(0, 0), ImVec2(1, 1), ImColor(tint));

                p0 = ImGui::GetCursorScreenPos();
                p1 = ImVec2(p0.x + height - 100, p0.y + height - 100);
                tint = ImVec4(1.0f, 1.0f, 1.0f, maskTrans);
                drawList->AddImage((void*)(intptr_t)maskLayerTexture, p0, p1, ImVec2(0, 0), ImVec2(1, 1), ImColor(tint));
                ImGui::SetCursorScreenPos(ImVec2(p0.x, p1.y));
            }
            if (ImGui::SliderInt("Layer number", &layerNumber, 0, itrEnd, "")) {
                checkLimits(layerNumber, 0, itrEnd);
                glDeleteTextures(1, &imageLayerTexture);
                imageLayerTexture = convertMatToTexture(*(itr + layerNumber));
                glDeleteTextures(1, &maskLayerTexture);
                maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
            };
            ImGui::SetNextItemWidth(100);
            if (!maskOn)
                ImGui::SetCursorPosX(columnWidth1 * 0.2);
            else
                ImGui::SetCursorPosX(columnWidth1 * 0.01);
            if (ImGui::InputInt(" ", &layerNumber, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
                checkLimits(layerNumber, 0, itrEnd);
                glDeleteTextures(1, &imageLayerTexture);
                imageLayerTexture = convertMatToTexture(*(itr + layerNumber));
                glDeleteTextures(1, &maskLayerTexture);
                maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
            };
            ImGui::SameLine();
            ImGui::Checkbox("Mask On", &maskOn);
            if (maskOn) {
                ImGui::PushItemWidth(150);
                ImGui::SameLine();
                ImGui::InputFloat("Layer Transparency", &oriTrans, 0.01f, 1.0f, "%.2f");
                ImGui::SameLine();
                ImGui::InputFloat("Mask Transparency", &maskTrans, 0.01f, 1.0f, "%.2f");
                ImGui::PopItemWidth();
                if (oriTrans < 0.0) {
                    oriTrans = 0.0;
                }
                else if (oriTrans > 1.0) {
                    oriTrans = 1.0;
                }
                if (maskTrans < 0.0) {
                    maskTrans = 0.0;
                }
                else if (maskTrans > 1.0) {
                    maskTrans = 1.0;
                }
            }
            ImGui::NextColumn();
            ImGui::PushItemWidth(150);
            ImGui::Text("Filter Type: ");
            ImGui::SameLine();
            if (ImGui::Combo("##combo", &currentFilter, filters, IM_ARRAYSIZE(filters))) {
                switch (currentFilter) {
                case 0:
                    config.filter = ImageProcessor::Settings::BILATERAL;
                    break;
                case 1:
                    config.filter = ImageProcessor::Settings::NLM;
                    break;
                case 2:
                    config.filter = ImageProcessor::Settings::NONE;
                    break;
                }
            };
            if (currentFilter != 2) {
                if (ImGui::InputInt("filterIterations", &filterIterations, 1, 5)) {
                    checkLimits(filterIterations, 1, 5);
                    config.filterIterations = filterIterations;
                };
                if (ImGui::InputInt("ksize", &ksize, 0, 0)) {
                    if (ksize < 0) {
                        ksize = 1;
                    }
                    else if (ksize > 29) {
                        ksize = 29;
                    }
                    else if (ksize % 2 == 0) {
                        ksize = ksize + 1;
                    }
                    config.ksize = ksize;
                    segment.updateConfig(config);
                };
                if (currentFilter == 1) {
                    if (ImGui::InputInt("filterhParam", &filterhParam, 1, 100)) {
                        checkLimits(filterhParam, 5, 100);
                        config.filterhParam = filterhParam;
                    };
                }
            }

            ImGui::NewLine();
            ImGui::Text("Threshold Type: ");
            ImGui::SameLine();
            if (ImGui::Combo("##combo2", &currentThreshold, thresholds, IM_ARRAYSIZE(thresholds))) {
                switch (currentThreshold) {
                case 0:
                    config.thresholdMethod = ImageProcessor::Settings::BINARY;
                    break;
                case 1:
                    config.thresholdMethod = ImageProcessor::Settings::OTSU;
                    break;
                case 2:
                    config.thresholdMethod = ImageProcessor::Settings::MEAN_STD_DEV;
                    break;
                case 3:
                    config.thresholdMethod = ImageProcessor::Settings::KAPUR;
                    break;
                }
            };
            if (currentThreshold == 2) {
                if (ImGui::InputInt("bdc", &bdc, 1, 3)) {
                    checkLimits(bdc, 0, 3);
                    config.BDC = bdc;
                };
            }

            if (currentThreshold == 0) {
                if (ImGui::InputInt("threshold", &threshold, 0, 255)) {
                    checkLimits(threshold, 0, 255);
                    config.threshold = threshold;
                };
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.8f, 1.0f));
            if (ImGui::Button("Try these parameters")) {
                segment.updateConfig(config);
                segment.readConfig();
                paramRes = segment.createMasks(paramPics, config);
                glDeleteTextures(1, &paramResult);
                paramResult = convertMatToTexture(modifyColors(paramRes[0]));
            }
            ImGui::PopStyleColor();
            ImGui::PopItemWidth();

            ImGui::NewLine();
            ImGui::Checkbox("Lungs analysis mode", &mode);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.8f, 1.0f));
            ImGui::PushItemWidth(150);
            if (ImGui::Button("Semi-Automatic Marking")) {
                glfwSetCursor(window, customCursor);
                masksImages = segment.createMasks(layerImages, config);
                itrm = masksImages.begin();
                itrmEnd = masksImages.size() - 1;
                glDeleteTextures(1, &maskLayerTexture);
                maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
                glfwSetCursor(window, NULL);
            }
            ImGui::PopStyleColor();

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));

            if (ImGui::Button("Analysis")) {
                glfwSetCursor(window, customCursor);
                poroInfo.UpdateObject(masksImages);
                GetData();
                glfwSetCursor(window, NULL);
                show_processed_window = true;
                /*
                 mats = GetMarkupError();
                 if (mats.size() != 0) {
                     markup_error = true;
                 }
                 else {
                     GetData();
                     show_processed_window = true;
                 }
                 */
            }

            ImGui::PopItemWidth();
            ImGui::PopStyleColor();

            ImGui::NewLine();
            ImGui::NewLine();
            ImGui::Text("Original picture:");
            ImGui::SameLine(350.0f * width / 1920);
            ImGui::Text("After segmentation:");
            ImGui::NewLine();
            ImGui::Image((void*)(intptr_t)paramTexture, ImVec2(300 * width / 1920, 300 * width / 1920));
            ImGui::SameLine(350.0f * width / 1920);
            ImGui::Image((void*)(intptr_t)paramResult, ImVec2(300 * width / 1920, 300 * width / 1920));

            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey::ImGuiKey_KeypadAdd))) {
                layerNumber++;
                checkLimits(layerNumber, 0, itrEnd);
                glDeleteTextures(1, &imageLayerTexture);
                imageLayerTexture = convertMatToTexture(*(itr + layerNumber));
                glDeleteTextures(1, &maskLayerTexture);
                maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
            }
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey::ImGuiKey_KeypadSubtract))) {
                layerNumber--;
                checkLimits(layerNumber, 0, itrEnd);
                glDeleteTextures(1, &imageLayerTexture);
                imageLayerTexture = convertMatToTexture(*(itr + layerNumber));
                glDeleteTextures(1, &maskLayerTexture);
                maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
            }
            ImGui::End();
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (markup_error) {
            SolveMarkupErrors(mats);
        }

        if (show_processed_window) {
            ProcessedWindow();
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if (showDummyWindow) {
            ImGui::OpenPopup("Dummy Window");
            if (ImGui::BeginPopupModal("Dummy Window", &showDummyWindow, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::SetNextWindowSize(ImVec2(400, 0));
                ImGui::Text("This button works!");
                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    showDummyWindow = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        dirDialog.Display();
        if (dirDialog.HasSelected())
        {
            bool dirError;
            std::vector<std::filesystem::path> selectedDir = dirDialog.GetMultiSelected();
            layerImages.clear();
            masksImages.clear();
            dir_loader.reset();
            for (const auto& path : selectedDir) {
                ve::Error err = dir_loader.loadFromDirectory(path);
                errorMessage = err.message;
                dirError = err;
            }
            if (!dirError)
            {
                showErrorPopup = true;
            }
            try {
                layerImages = dir_loader.copyData();
                masksImages.push_back(cv::imread(picsPath + "maskexample.png", cv::IMREAD_GRAYSCALE));
                masksImages.resize(layerImages.size(), cv::imread(picsPath + "maskexample.png", cv::IMREAD_GRAYSCALE));
            }
            catch (const std::exception& e) {
                errorMessage = e.what();
                showErrorPopup = true;
            }
            if (!layerImages.empty()) {
                itr = layerImages.begin();
                itrEnd = layerImages.size() - 1;
                itrm = masksImages.begin();
                itrmEnd = masksImages.size() - 1;
                glDeleteTextures(1, &imageLayerTexture);
                imageLayerTexture = convertMatToTexture(*itr);
                glDeleteTextures(1, &maskLayerTexture);
                maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
            }
            else {
                layerImages.push_back(cv::imread(picsPath + "nolayerpic.png", cv::IMREAD_COLOR));
                masksImages.push_back(cv::imread(picsPath + "maskexample.png", cv::IMREAD_GRAYSCALE));
                itr = layerImages.begin();
                itrEnd = layerImages.size() - 1;
                itrm = masksImages.begin();
                itrmEnd = masksImages.size() - 1;
                glDeleteTextures(1, &imageLayerTexture);
                imageLayerTexture = convertMatToTexture(*itr);
                glDeleteTextures(1, &maskLayerTexture);
                maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
            }
            dirDialog.ClearSelected();
        }

        fileDialog.Display();
        if (fileDialog.HasSelected())
        {
            std::vector<std::filesystem::path> selectedFiles = fileDialog.GetMultiSelected();
            layerImages.clear();
            masksImages.clear();
            dir_loader.reset();
            ve::Error err = dir_loader.loadFromFiles(selectedFiles.cbegin(), selectedFiles.cend());
            errorMessage = err.message;
            if (!err)
            {
                std::cout << err.message;
                showErrorPopup = true;
            }
            try {
                layerImages = dir_loader.copyData();
                masksImages.push_back(cv::imread(picsPath + "maskexample.png", cv::IMREAD_GRAYSCALE));
                masksImages.resize(layerImages.size(), cv::imread(picsPath + "maskexample.png", cv::IMREAD_GRAYSCALE));
            }
            catch (const std::exception& e) {
                errorMessage = e.what();
                showErrorPopup = true;
            }
            if (!layerImages.empty()) {
                itr = layerImages.begin();
                itrEnd = layerImages.size() - 1;
                itrm = masksImages.begin();
                itrmEnd = masksImages.size() - 1;
                glDeleteTextures(1, &imageLayerTexture);
                imageLayerTexture = convertMatToTexture(*itr);
                glDeleteTextures(1, &maskLayerTexture);
                maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
            }
            else {
                layerImages.push_back(cv::imread(picsPath + "nolayerpic.png", cv::IMREAD_COLOR));
                masksImages.push_back(cv::imread(picsPath + "maskexample.png", cv::IMREAD_GRAYSCALE));
                itr = layerImages.begin();
                itrEnd = layerImages.size() - 1;
                itrm = masksImages.begin();
                itrmEnd = masksImages.size() - 1;
                glDeleteTextures(1, &imageLayerTexture);
                imageLayerTexture = convertMatToTexture(*itr);
                glDeleteTextures(1, &maskLayerTexture);
                maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
            }
            fileDialog.ClearSelected();
        }

        projDirDialog.Display();
        if (projDirDialog.HasSelected()) {
            show_start_window = false;
            segment.setConfigFilePath(projDirDialog.GetSelected().string());
            segment.createDefaultConfig();
            config = segment.readConfig();
            paramRes = segment.createMasks(paramPics, config);
            glDeleteTextures(1, &paramResult);
            paramResult = convertMatToTexture(paramRes[0]);
            fs::create_directory(projDirDialog.GetSelected().string() + "/masks");
            show_project_window = true;
            projDirDialog.ClearSelected();
        }

        projOpenDirDialog.Display();
        if (projOpenDirDialog.HasSelected()) {
            segment.setConfigFilePath(projOpenDirDialog.GetSelected().string());
            if (segment.configExists()) {
                show_start_window = false;
                config = segment.readConfig();
                paramRes = segment.createMasks(paramPics, config);
                glDeleteTextures(1, &paramResult);
                paramResult = convertMatToTexture(paramRes[0]);
                show_project_window = true;
                projOpenDirDialog.ClearSelected();
            }
            else {
                showErrorOpenPopup = true;
                projOpenDirDialog.ClearSelected();
            }
        }

        if (showErrorPopup) {
            ImGui::OpenPopup("Error");
            if (ImGui::BeginPopupModal("Error", &showErrorPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::SetNextWindowSize(ImVec2(400, 0));
                ImGui::Text("An error occurred!");
                ImGui::TextWrapped(("Couldn't read an image from a file " + errorMessage).c_str());
                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    showErrorPopup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        if (showErrorOpenPopup) {
            ImGui::OpenPopup("Error");
            if (ImGui::BeginPopupModal("Error", &showErrorOpenPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::SetNextWindowSize(ImVec2(400, 0));
                ImGui::Text("An error occurred!");
                ImGui::TextWrapped("This directory is not a project directory");
                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    showErrorOpenPopup = false;
                    show_start_window = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        if (showExitPopup) {
            ImGui::OpenPopup("Exit Popup");
            if (ImGui::BeginPopupModal("Exit Popup", &showExitPopup, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
                ImGui::SetNextWindowSize(ImVec2(510, 0));
                ImGui::Text("Do you want to exit without saving the project?");
                if (ImGui::Button("Yes")) {
                    layerImages.clear();
                    masksImages.clear();
                    layerImages.push_back(cv::imread(picsPath + "nolayerpic.png", cv::IMREAD_COLOR));
                    masksImages.push_back(cv::imread(picsPath + "maskexample.png", cv::IMREAD_GRAYSCALE));
                    itr = layerImages.begin();
                    itrEnd = layerImages.size() - 1;
                    itrm = masksImages.begin();
                    itrmEnd = masksImages.size() - 1;
                    glDeleteTextures(1, &imageLayerTexture);
                    imageLayerTexture = convertMatToTexture(*itr);
                    glDeleteTextures(1, &maskLayerTexture);
                    maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
                    showExitPopup = false;
                    show_project_window = false;
                    show_start_window = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Save and Exit")) {
                    segment.updateConfig(config);
                    layerImages.clear();
                    masksImages.clear();
                    // Saving masks should be here
                    layerImages.push_back(cv::imread(picsPath + "nolayerpic.png", cv::IMREAD_COLOR));
                    masksImages.push_back(cv::imread(picsPath + "maskexample.png", cv::IMREAD_GRAYSCALE));
                    itr = layerImages.begin();
                    itrEnd = layerImages.size() - 1;
                    itrm = masksImages.begin();
                    itrmEnd = masksImages.size() - 1;
                    glDeleteTextures(1, &imageLayerTexture);
                    imageLayerTexture = convertMatToTexture(*itr);
                    glDeleteTextures(1, &maskLayerTexture);
                    maskLayerTexture = convertMatToTexture(modifyColors(*(itrm + layerNumber)));
                    showExitPopup = false;
                    show_project_window = false;
                    show_start_window = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("No")) {
                    showExitPopup = false;
                }
                ImGui::EndPopup();
            }
        }


        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
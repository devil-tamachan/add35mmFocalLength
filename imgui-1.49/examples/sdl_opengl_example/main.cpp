// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "sqlite3.h"


sqlite3_stmt *pGetNextUnsetModelStmt = NULL;
sqlite3_stmt *pUpdateModelStmt = NULL;
sqlite3_stmt *pUpdateSkipVideocamStmt = NULL;


static int lastmodelid = 1570;//-1;
static bool bDiagonal = false;
static double diagonal = 1.0;
static float sensorsize[] = {0.0, 0.0};
static int cameratype = 0;
static int r_cameratype = 0;
static char *lpFilename = NULL;
static char *lpModelname = NULL;

#define MYSAFERELEASE(p) {if(p){free(p);p=NULL;}}
#define MYSTRCLONE(dst,src) {MYSAFERELEASE(dst);dst = (char *)malloc(strlen(src)+2);strcpy(dst, src);}

void NextUnsetItem()
{
	int rc = 0;
	
	sqlite3_bind_int(pGetNextUnsetModelStmt, 1, lastmodelid);
	do {
		rc = sqlite3_step(pGetNextUnsetModelStmt);
		if(rc==SQLITE_ROW)
		{
			lastmodelid = sqlite3_column_int(pGetNextUnsetModelStmt, 0);
			if(sqlite3_column_type(pGetNextUnsetModelStmt, 1)==SQLITE_NULL)bDiagonal=false;
			else {
				bDiagonal=true;
				diagonal = sqlite3_column_double(pGetNextUnsetModelStmt, 1);
			}
			if(sqlite3_column_type(pGetNextUnsetModelStmt, 2)==SQLITE_NULL)cameratype=0;
			else cameratype = sqlite3_column_int(pGetNextUnsetModelStmt, 2);
			const char *file = (const char *)sqlite3_column_text(pGetNextUnsetModelStmt, 3);
			MYSTRCLONE(lpFilename, file);
			
			const char *mo = (const char *)sqlite3_column_text(pGetNextUnsetModelStmt, 4);
			MYSTRCLONE(lpModelname, mo);
			break;
		}
		if(rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED ){
			sqlite3_sleep(5);
		}
	} while(rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED);
	sqlite3_reset(pGetNextUnsetModelStmt);
}

double CalcDiagonal()
{
	double w = sensorsize[0];
	double h = sensorsize[1];
	return sqrt(w*w + h*h);
}

void UpdateModel()
{
	if(lastmodelid==-1)return;
	int rc = 0;
	
	sqlite3_bind_double(pUpdateModelStmt, 1, CalcDiagonal());
	sqlite3_bind_double(pUpdateModelStmt, 2, sensorsize[0]);
	sqlite3_bind_double(pUpdateModelStmt, 3, sensorsize[1]);
	sqlite3_bind_int(pUpdateModelStmt, 4, r_cameratype);
	sqlite3_bind_int(pUpdateModelStmt, 5, lastmodelid);
	do {
		rc = sqlite3_step(pUpdateModelStmt);
		if(rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED ){
          sqlite3_sleep(5);
		}
	} while(rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED);
	sqlite3_reset(pUpdateModelStmt);
}

void UpdateSkipVideocam()
{
	if(lastmodelid==-1)return;
	int rc = 0;
	
	sqlite3_bind_int(pUpdateSkipVideocamStmt, 1, 2);
	sqlite3_bind_int(pUpdateSkipVideocamStmt, 2, lastmodelid);
	do {
		rc = sqlite3_step(pUpdateSkipVideocamStmt);
		if(rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED ){
          sqlite3_sleep(5);
		}
	} while(rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED);
	sqlite3_reset(pUpdateSkipVideocamStmt);
}

struct SENSORLIST{
	const char *label;
	float sensorwidth;
	float sensorheight;
}sensorlist[]={
	{"cyuban (43.8, 32.8 mm)", 43.8f, 32.8},
	{"cyuban2 (44, 33 mm)", 44, 33},
	{"full (36, 24 mm)", 36, 24},
	{"APS-C (23.6, 15.8 mm)", 23.6, 15.8},
	{"APS-C[Canon] (22.3, 14.9 mm)", 22.3, 14.9},
	{"1.5[Canon] (18.7, 14 mm)", 18.7, 14},
	{"4/3 (17.3, 13 mm)", 17.3, 13},
	{"1[CX] (13.2, 8.8 mm)", 13.2, 8.8},
	{"2/3 (8.8, 6.6 mm)", 8.8, 6.6},
	{"1/1.6 (8.08, 6.01 mm)", 8.08, 6.01},
	{"1/1.7 (7.5, 5.6 mm)", 7.5, 5.6},
	{"1/1.76 (7.27, 5.46 mm)", 7.27, 5.46},
	{"1/1.8 (7.1, 5.4 mm)", 7.1, 5.4},
	{"1/2 (6.4, 4.8 mm)", 6.4, 4.8},
	{"1/2.3 (6.2, 4.7 mm)", 6.2, 4.7},
	{"1/2.33 (6.08, 4.56 mm)", 6.08, 4.56},
	{"1/2.35 (6.03, 4.52 mm)", 6.03, 4.52},
	{"1/2.4 (5.9, 4.43 mm)", 5.9, 4.43},
	{"1/2.5 (5.75, 4.32 mm)", 5.75, 4.32},
	{"1/2.6 (5.5, 4.1 mm)", 5.5, 4.1},
	{"1/2.7 (5.3, 4 mm)", 5.3, 4},
	{"1/2.8 (5.1, 3.8 mm)", 5.1, 3.8},
	{"1/3 (4.8, 3.6 mm)", 4.8, 3.6},
	{"1/3.2 (4.4, 3.3 mm)", 4.4, 3.3},
	{"1/3.9 (3.4, 2.5 mm)", 3.4, 2.5},
	{"1/4 (3.2, 2.4 mm)", 3.2, 2.4},
	{"1/5 (2.9, 2.2 mm)", 2.9, 2.2},
	{"1/6 (2.4, 1.8 mm)", 2.4, 1.8}
};

void DrawCameraModels()
{
	
	if (ImGui::Begin("Cameras"))
	{
//		ImGui::BeginChild("S1", ImVec2(ImGui::GetWindowContentRegionWidth(),ImGui::GetWindowContentRegionHeight()), false, ImGuiWindowFlags_HorizontalScrollbar);
		{
			if (lastmodelid==-1)NextUnsetItem();
			ImGui::Separator();
			ImGui::Text("Sensor size:");
			int num_sensorlist = sizeof(sensorlist)/sizeof(SENSORLIST);
			for(int i=0; i<num_sensorlist;i++)
			{
				if(i!=0 && i%3!=0)ImGui::SameLine();
				if(ImGui::Button(sensorlist[i].label))
				{
					sensorsize[0] = sensorlist[i].sensorwidth;
					sensorsize[1] = sensorlist[i].sensorheight;
				}
			}
			ImGui::InputFloat2("Sensor size", sensorsize, 2/*printf %.Xf*/);
			ImGui::Separator();
			ImGui::Text("type:");
			ImGui::RadioButton("Camera", &r_cameratype, 0); ImGui::SameLine();
			ImGui::RadioButton("Phone", &r_cameratype, 1); ImGui::SameLine();
			ImGui::RadioButton("Videocam", &r_cameratype, 2);
			ImGui::Separator();
			if (ImGui::Button("Update"))
			{
				UpdateModel();
				NextUnsetItem();
			}
			ImGui::SameLine();
			if (ImGui::Button("VideocamSkip"))
			{
				UpdateSkipVideocam();
				NextUnsetItem();
			}
			ImGui::SameLine();
			if (ImGui::Button("Next"))
			{
				NextUnsetItem();
			}
			ImGui::Separator();
		
			ImGui::Text("id: %d", lastmodelid);
			ImGui::Text("filename: %s", lpFilename);
			ImGui::Text("modelname: %s", lpModelname);
			ImGui::Text("diagonal: %lf", diagonal);
			ImGui::Text("type: %d", cameratype);
			ImGui::Separator();
		}
//		ImGui::EndChild();
		
	}
	ImGui::End();
}

static bool done = false;



#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("ImGui SDL2+OpenGL example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    // Setup ImGui binding
    ImGui_ImplSdl_Init(window);

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);

    bool show_test_window = true;
    bool show_another_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);
	
	
    Uint64 end_counter = 0;
	
	
	const char *zFilename = "../c.sqlite";
	sqlite3 *pDb = NULL;
	char *errMsg = NULL;
	
	int rc = sqlite3_open(zFilename, &pDb);
	if(rc != SQLITE_OK)
	{
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);errMsg = NULL;
		goto main_ERR1;
	}
	
	rc = sqlite3_prepare_v2(pDb, "SELECT models.id, models.diagonal, models.type, models.filename, models.name FROM models INNER JOIN makers ON models.makerid = makers.id WHERE models.diagonal IS NULL AND models.id > ?1 AND (models.type IS NULL OR models.type!=2) LIMIT 1;", -1, &pGetNextUnsetModelStmt, NULL);
	if(rc != SQLITE_OK)
	{
		printf("208: %s\n", sqlite3_errmsg(pDb));
		sqlite3_finalize(pGetNextUnsetModelStmt);
		goto main_ERR1;
	}
	
	
	rc = sqlite3_prepare_v2(pDb, "UPDATE models SET diagonal = ?1, sensorwidth = ?2, sensorheight = ?3, type = ?4 WHERE id = ?5;", -1, &pUpdateModelStmt, NULL);
	if(rc != SQLITE_OK)
	{
		printf("217: %s\n", sqlite3_errmsg(pDb));
		sqlite3_finalize(pUpdateModelStmt);
		goto main_ERR1;
	}
	
	rc = sqlite3_prepare_v2(pDb, "UPDATE models SET type = ?1 WHERE id = ?2;", -1, &pUpdateSkipVideocamStmt, NULL);
	if(rc != SQLITE_OK)
	{
		printf("217: %s\n", sqlite3_errmsg(pDb));
		sqlite3_finalize(pUpdateSkipVideocamStmt);
		goto main_ERR1;
	}
	
	
    // Main loop
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdl_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }
        ImGui_ImplSdl_NewFrame(window);
		
		

		
        ImGui::SetNextWindowSize(ImVec2(100,300), ImGuiSetCond_FirstUseEver);
		DrawCameraModels();

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        /*if (show_test_window)
        {*/
        //    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        //    ImGui::ShowTestWindow(&show_test_window);
        /*}*/
        

        // Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        SDL_GL_SwapWindow(window);
        
        double kankaku = 1.0 / 60.0;
        kankaku *= SDL_GetPerformanceFrequency();
        Uint64 kankakuU64 = kankaku;
        Uint64 keika = SDL_GetPerformanceCounter() - end_counter;
        if(kankakuU64 > keika)
        {
          Uint64 nokoriU64 = kankakuU64 - keika;
          double nokori = nokoriU64;
          nokori /= SDL_GetPerformanceFrequency();
          SDL_Delay(nokori*1000.0);
        }
        end_counter = SDL_GetPerformanceCounter();
        SDL_WaitEvent(NULL);
	}
	
main_ERR1:
	sqlite3_finalize(pGetNextUnsetModelStmt);
	sqlite3_finalize(pUpdateModelStmt);
	sqlite3_finalize(pUpdateSkipVideocamStmt);
	sqlite3_close(pDb);

    // Cleanup
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#include "stdafx.h"
#include "Scene.h"

#include "Framework.h"
#include "Csv.h"
#include "Timer.h"

Scene g_Scene;

static ESceneType s_nextScene = SCENE_NULL;

#pragma region TitleScene

#define SOLID 0
#define SHADED 1
#define BLENDED 2

typedef struct TitleSceneData
{
	Text   GuideLine[1];
	Music   TitleBGM;
	float   Volume;
	Text   EnterText;
	Text   LoadingText;
	int32   FontSize;
	int32   RenderMode;
	Image   TitleImage;
	int32   X;
	int32   Y;
	Image   StartImage;
	int32   SX;
	int32   SY;
	Image    BlackOutImage;
	int32    BlackOutAlpha;
} TitleSceneData;

bool Loading = false;
void init_title(void)
{
	g_Scene.Data = malloc(sizeof(TitleSceneData));
	memset(g_Scene.Data, 0, sizeof(TitleSceneData));

	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;

	data->FontSize = 48;
	Text_CreateText(&data->EnterText, "GmarketSansTTFBold.ttf", data->FontSize, L"--- PRESS THE SPACE ---", 24);
	Text_CreateText(&data->LoadingText, "GmarketSansTTFBold.ttf", data->FontSize, L"----- LOADING -----", 24);

	data->RenderMode = SOLID;

	Image_LoadImage(&data->TitleImage, "title_2.jpg");
	data->X = 0;
	data->Y = 0;
	Image_LoadImage(&data->StartImage, "start.png");
	data->SX = (WINDOW_WIDTH / 2) - (data->StartImage.Width / 2);
	data->SY = 650;

	Audio_LoadMusic(&data->TitleBGM, "title.mp3");

	Audio_PlayFadeIn(&data->TitleBGM, INFINITY_LOOP, 3000);

	data->Volume = 1.0f;

	data->BlackOutAlpha = 255;
	Image_LoadImage(&data->BlackOutImage, "black.jpg");
	Image_SetAlphaValue(&data->BlackOutImage, data->BlackOutAlpha);

	Loading = false;
}

void update_title(void)
{
	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;
	if (data->BlackOutAlpha > 0)
	{
		data->BlackOutAlpha -= 5;
	}
	if (data->BlackOutAlpha <= 0)
	{
		if (Input_GetKeyDown(VK_SPACE))
		{
			Loading = true;
			Scene_SetNextScene(SCENE_CREDIT);
		}
	}
	Image_SetAlphaValue(&data->BlackOutImage, data->BlackOutAlpha);
}
void render_title(void)
{
	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;
	Renderer_DrawImage(&data->TitleImage, data->X, data->Y);
	static float elapsedTime = 1.0f;

	SDL_Color color = { .a = 255 };
	static bool isShow = true;
	elapsedTime += Timer_GetDeltaTime();
	if (!Loading)
	{
		if (elapsedTime >= 1.0f)
		{
			Renderer_DrawImage(&data->StartImage, data->SX, data->SY);
			Renderer_DrawTextSolid(&data->EnterText, WINDOW_WIDTH / 2 - (data->EnterText.Length * data->FontSize) / 4, 950, color);

			if (elapsedTime >= 1.5f)
			{
				elapsedTime = 0.0f;
			}
		}
	}
	else
	{
		color.r = 255;
		color.g = 255;
		color.b = 255;
		isShow = false;
		Renderer_DrawTextSolid(&data->LoadingText, WINDOW_WIDTH / 2 - (data->LoadingText.Length * data->FontSize) / 4 + 20, 820, color);
	}

}

void release_title(void)
{
	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;

	//for (int32 i = 0; i < 10; ++i)
	//{
	//   Text_FreeText(&data->GuideLine[i]);
	//}
	Text_FreeText(&data->EnterText);
	SafeFree(g_Scene.Data);
}
#pragma endregion

#define TextFont "GmarketSansTTFLight.ttf"

#pragma region SceneData
//BGM 관련
enum BGMType {
	BGM_TITLE,
	BGM_MA_E2,
	BGM_IPARK_START,
	BGM_GAME_CENTER,
	BGM_3ED,
	BGM_ENDING_HAPPY,
	BGM_ENDING_SAD,
	BGM_ZOMBIES,
	BGM_BSIN,
	BGM_BEGAUN_CUNG,
	BGM_BEGAUN_UNG,
	BGM_DUGENG,
	BGM_KIZUNGUNG,
	BGM_IPARK_START_DRUM,
	BGM_IPARK_START_RAT,
	BGM_IPARK_SHEEEIC,
	BGM_JUNGHU
};

#define MAX_BG_CHANGE_COUNT 4		//배경 이미지 최대로 바뀌는 갯수
#define MAX_TEXT_SET_COUNT 13		//텍스트 세트가 최대로 바뀌는 갯수
#define MAX_TEXT_COUNT 7			//각 텍스트 세트에 최대 줄갯수
#define MAX_OPTION_COUNT 3			//선택지 최대 갯수
//다른 정보를 받아올 때마다 추가될 가능성 있음

//각 씬의 정보를 담든 structure
typedef struct tagScene {
	int32			SceneNumber;								//씬 넘버
	Text			SceneName;									//씬 이름

	Image			BGImage;									//배경화면
	Image			AdditionBGChangeImages[MAX_BG_CHANGE_COUNT];//배경 추가 이미지 배열
	int32			AddImageTimings[MAX_BG_CHANGE_COUNT];		//배경 추가 이미지 타이밍

	Music			BGM;										//배경 음악
	int32			BGMNumber;									//배경 음악 번호 위에 열거형에 정의

	Image			ItemImage;									//아이템 이미지
	int32			AddItemImageTiming;							//아이템 이미지 등장 타이밍
	int32			FadeItemImageTiming;						//아이템 이미지 사라지는 타이밍

	int32			AddPoundingItemImageTiming;					//아이템 이미지 두근 거리는 효과 넣는 타이밍
	int32			FadePoundingItemImageTiming;				//아이템 이미지 두근 거리는 효과 그만하는 타이밍

	int32			ImagePushingType;							//이미지(화면, 아이템) 밀기 유형(-1: 없음, 0: 배경만, 1: 아이템만)
	int32			ImagePushingTiming;							//이미지 밀기 시작 타이밍
	int32			ImagePushingX;								//이미지 민 후 변화한 x값
	int32			ImagePushingY;								//이미지 민 후 변화한 y값

	int32			ShakingTimging;								//화면 흔들기 타이밍
	int32			ShakingX;									//화면 흔들기에서 사용할 X값
	int32			ShakingY;									//화면 흔들기에서 사용할 X값

	int32			DialogCount;									//텍스트 갯수
	Text			DialogList[MAX_TEXT_SET_COUNT][MAX_TEXT_COUNT];	//텍스트 배열

	int32			OptionCount;								//옵션 갯수
	Text			OptionList[MAX_OPTION_COUNT];				//옵션 텍스트 배열

	int32			NextSceneNumberList[MAX_OPTION_COUNT];		//옵션 선택시 넘어가는 씬 넘버

	bool			isShowedThisEnding;							//엔딩을 봤는지(엔딩 씬일 경우에만 사용)
} SceneStruct;

void GetSceneData(int32 sceneNum, SceneStruct* scene) {
	CsvFile csv;
	memset(&csv, 0, sizeof(CsvFile));
	CreateCsvFile(&csv, "csv_ver3_temp.csv");

	if (csv.Items[sceneNum] == NULL) {
		printf("ERROR!!! WORNG SCENE NUMBER");
		return;
	}

	int32 columCount = 0;

	//씬 번호
 	scene->SceneNumber = ParseToInt(csv.Items[sceneNum][columCount++]);
	//씬 이름
	wchar_t* SceneNameTemp = ParseToUnicode(csv.Items[sceneNum][columCount++]);
	Text_CreateText(&scene->SceneName, TextFont, 25, SceneNameTemp, wcslen(SceneNameTemp));
	//bg Image
	Image_LoadImage(&scene->BGImage, ParseToAscii(csv.Items[sceneNum][columCount++]));
	//bgm
	//Audio_LoadMusic(&Scenes[sceneNum].BGM, ParseToAscii(csv.Items[i][columCount++]));
	scene->BGMNumber = ParseToInt(csv.Items[sceneNum][columCount++]);

	//배경 전환 이미지
	for (int32 j = 0; j < MAX_BG_CHANGE_COUNT;j++) {
		int32 additionalImagePoint = ParseToInt(csv.Items[sceneNum][columCount + 1]);
		scene->AddImageTimings[j] = additionalImagePoint;
		if (additionalImagePoint > -1) {
			Image_LoadImage(&scene->AdditionBGChangeImages[j], ParseToAscii(csv.Items[sceneNum][columCount]));
		}
		columCount += 2;
	}

	//아이템 이미지
	int32 itemImagePoint = ParseToInt(csv.Items[sceneNum][columCount + 1]);
	scene->AddItemImageTiming = itemImagePoint;
	if (itemImagePoint > -1) {
		Image_LoadImage(&scene->ItemImage, ParseToAscii(csv.Items[sceneNum][columCount]));
		scene->FadeItemImageTiming = ParseToInt(csv.Items[sceneNum][columCount + 2]);
	}
	columCount += 3;

	//아이템 두근두근 효과
	int32 poundingPoint = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->AddPoundingItemImageTiming = poundingPoint;
	if (poundingPoint > -1) {
		scene->FadePoundingItemImageTiming = ParseToInt(csv.Items[sceneNum][columCount]);
	}
	columCount++;

	//화면 흔들기 효과
	scene->ShakingTimging = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->ShakingX = 0;
	scene->ShakingY = 0;

	//화면 밀기 효과(수정 요함!)
	//scene->ImagePushingTiming = ParseToInt(csv.Items[sceneNum][columCount++]);
	//scene->ImagePushingType
	scene->ImagePushingType = -1;
	scene->ImagePushingTiming = -1;
	scene->ImagePushingX = 0;
	scene->ImagePushingY = 0;
	columCount += 2;

	//텍스트 데이터 저장
	int32 dialogCount = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->DialogCount = dialogCount;
	for (int32 j = 0; j < MAX_TEXT_SET_COUNT;j++) {
		if (dialogCount > j) {
			wchar_t* originalData = ParseToUnicode(csv.Items[sceneNum][columCount]);
			wchar_t lineData[2048];
			int32 originalIndex = 0, lineDataIndex = 0, dialogListIndex = 0;
			wchar_t preChar = '\0';

			//텍스트 줄바꿈 단위로 나눠 저장
			while (true) {
				//그냥 입력일 경우
				if (*(originalData + originalIndex) != '\n' && *(originalData + originalIndex) != '\0') {
					//"가 아닐 경우 그냥 넣어준다.
					if (*(originalData + originalIndex) != '\"') {
						lineData[lineDataIndex++] = *(originalData + originalIndex);
						preChar = *(originalData + originalIndex);
					}
					// "가 연속으로 있을 경우 넣어준다.
					else {
						if (preChar == *(originalData + originalIndex)) {
							lineData[lineDataIndex++] = *(originalData + originalIndex);
							preChar = '\0';
						}
						else {
							preChar = *(originalData + originalIndex);
						}
					}
				}
				//개행이거나 널일 경우 text를 넣어준다.
				else {
					lineData[lineDataIndex++] = '\0';
					Text_CreateText(&scene->DialogList[j][dialogListIndex], "GmarketSansTTFLight.ttf", 25, lineData, wcslen(lineData));
					dialogListIndex++;
					lineDataIndex = 0;

					if (*(originalData + originalIndex) == '\0') {
						break;
					}
				}
				originalIndex++;
			}

			Text_CreateText(&scene->DialogList[j][dialogListIndex], "GmarketSansTTFLight.ttf", 30, L"", wcslen(L""));
		}
		columCount++;
	}

	//옵션 데이터 저장
	scene->OptionCount = ParseToInt(csv.Items[sceneNum][columCount++]);

	//만약 선택지가 없으면 다음 씬과 자동으로 연결시켜준다.
	if (scene->OptionCount > 0) {
		for (int32 j = 0; j < MAX_OPTION_COUNT;j++) {
			if (scene->OptionCount > j) {
				wchar_t* temp = ParseToUnicode(csv.Items[sceneNum][columCount]);
				Text_CreateText(&scene->OptionList[j], "GmarketSansTTFLight.ttf", 20, temp, wcslen(temp));

				columCount++;
				scene->NextSceneNumberList[j] = ParseToInt(csv.Items[sceneNum][columCount]);
				columCount++;
			}
		}
	}
	else {
		scene->NextSceneNumberList[0] = ParseToInt(csv.Items[sceneNum][++columCount]);
	}

	scene->isShowedThisEnding = false;

	FreeCsvFile(&csv);
}

void Scene_Clear(SceneStruct* scene) {
	//이미지 해제
	Image_FreeImage(&scene->BGImage);
	for (int32 j = 0; j < MAX_BG_CHANGE_COUNT;j++) {
		if (scene->AddImageTimings[j] > -1) {
			Image_FreeImage(&scene->AdditionBGChangeImages[j]);
		}
		else {
			break;
		}
	}
	Image_FreeImage(&scene->ItemImage);

	//오디오 해제
	Audio_FreeMusic(&scene->BGM);

	//텍스트 해제
	if (scene->SceneName.Length > 0) {
		Text_FreeText(&scene->SceneName);
	}
	for (int32 j = 0; j < scene->DialogCount;j++) {
		int32 k = 0;
		while (scene->DialogList[j][k].Length > 0) {
			Text_FreeText(&scene->DialogList[j][k]);
			k++;
		}
		Text_FreeText(&scene->DialogList[j][k]);
	}
	for (int32 j = 0; j < scene->OptionCount;j++) {
		Text_FreeText(&scene->OptionList[j]);
	}
}

#pragma endregion

#pragma region CreditScene
typedef struct CreditSceneData
{
	Text	EnterText;
	int32	FontSize;
	int32	RenderMode;
	TitleSceneData TitleBGM;
	Image	CreditImage[3];
	int32	X;
	int32	Y;
} CreditSceneData;

int32 s_CurrentPageCredit;

void init_credit(void)
{
	g_Scene.Data = malloc(sizeof(CreditSceneData));
	memset(g_Scene.Data, 0, sizeof(CreditSceneData));

	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;

	data->FontSize = 48;
	Text_CreateText(&data->EnterText, "chosun.ttf", data->FontSize, L"--- PRESS THE SPACE ---", 24);

	data->RenderMode = SOLID;

	Image_LoadImage(&data->CreditImage[0], "credit.jpg");
	Image_LoadImage(&data->CreditImage[1], "1.jpg");
	Image_LoadImage(&data->CreditImage[2], "1-4.jpg");
	data->X = 0;
	data->Y = 0;

	s_CurrentPageCredit = 0;
}

void update_credit(void)
{
	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;

	if (Input_GetKeyDown(VK_SPACE))
	{
		if(s_CurrentPageCredit < 2){
			s_CurrentPageCredit++;
		}
		else {
			Scene_SetNextScene(SCENE_MAIN);
			Audio_Stop();
		}
	}
}

void render_credit(void)
{
	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;
	Renderer_DrawImage(&data->CreditImage[s_CurrentPageCredit], data->X, data->Y);
	static float elapsedTime = 0;

	static bool isShow = true;
	elapsedTime += Timer_GetDeltaTime();
	if (elapsedTime >= 1.0f)
	{
		SDL_Color color = { .a = 255 };
		Renderer_DrawTextSolid(&data->EnterText, WINDOW_WIDTH / 2 - (data->EnterText.Length * data->FontSize) / 4, 900, color);

		if (elapsedTime >= 2.0f)
		{
			elapsedTime = 0.0f;
		}
	}

}

void release_credit(void)
{
	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;

	Text_FreeText(&data->EnterText);
	Audio_FreeMusic(&data->TitleBGM);

	Image_FreeImage(&data->CreditImage[0]);
	Image_FreeImage(&data->CreditImage[1]);
	Image_FreeImage(&data->CreditImage[2]);


	SafeFree(g_Scene.Data);
}
#pragma endregion

#pragma region Intro

const wchar_t* intro1str[] = {
	L"한차례 퍼진 '좀비 병'의 유행으로 국가 멸망위기 까지 갔던 대한민국은, ",
	L"선진국들의 도움으로 '좀비 바이러스'를 퇴치하고 국가 정상화에 돌입했다.",
	L" ",
	L"더 이상의 '좀비병'으로 인한 나라의 멸망을 두고 볼 수 없던 대통령은",
	L" 대통령 산하 TF팀 <행복 연구소> 를 설립했다.",
};
const wchar_t* intro2str[] = {
	L"대통령 산하 TF팀 <행복 연구소> 에서는 ",
	L"국민 여러분의 행복과 안보를 책임질 여러 연구를 하고 있습니다.",
	L" ",
	L"여러분들의 건강과 행복을 ",
	L"질병과 바이러스로부터지켜드리기 위하여 ",
	L" ",
	L"<행복 연구소> 는 정부 기관 아래 최첨단 연구 장비와 지식을 겸비하여",
	L"꾸준한 연구 및 개발을 진행하고 있습니다.",
};
const wchar_t* intro3str[] = {
	L"하지만 사실, 이 <행복 연구소> 는 ",
	L"<좀비 바이러스를 이용한 대량 살상 무기>를 개발하는 곳이었고,  ",
	L"거기서 과학계에서 금지된 인간의 염색체를 ",
	L"이용한 지능 쥐 실험을 하고 있었다. 물론, 극비 사항이었다.",
	L" ",
	L"인간의 염색체와 뇌세포를 이식하여 지능을 높이고 ",
	L"최첨단 연구 장비와 지식을 겸비하여 ",
	L"꾸준한 연구 및 개발을 진행하고 있습니다. ",
};
const wchar_t* intro4str[] = {
	L"이 사실이 밝혀지면 안되는 정부는, \"좀비 바이러스\" 라고  ",
	L"질병 코드 000 제로 로 명명하고 전파 주의보를 내렸다.",
	L" ",
	L"<행복 연구소>에 '좀비 바이러스 의심자'를 신고하면,",
	L"포상금도 주기로 하였다. ",
	L"'좀비 바이러스 의심자'는 좀비화가 진행되는 상태이며 ",
	L"완전한 좀비는 아니기 때문에 ",
	L"<행복 연구소>로 가서 치료를 하면",
	L"'완전한 인간화'로 완치가 가능하다고 선전 하였다.",
	L" ",
	L"그렇기 때문에 모든 시민들은 \"좀비 바이러스\"  증상을 보이는 사람들을",
	L"국가에 신고하여 포상금을 받았다.",
};
const wchar_t* intro5str[] = {
	L"하지만 '좀비 바이러스 의심자' 괴담은 ",
	L"유투버 렉카 등을 통해 유투브에 퍼졌고, 그 내용은 이러했다.",
	L" ",
	L"1) '좀비 바이러스 의심자' 들이",
	L"<행복 연구소>를 들어가 나온 것을 본 사람이 없다. ",
	L"2) 실제 '좀비 바이러스 의심자'들은 연구에 쓰인다.",
	L"3) 가족들에게 이 사실을 발설 시 금액을 다시 회수한다는 조건으로",
	L"몇 십억 대의 금액을 주며동의서는 받아간다.",
	L" ",
	L"는 괴담이었다.",
};
const wchar_t* intro6str[] = {
	L"이 '지능 쥐'는 큰 부작용이 있었는데,",
	L"연구소 직원들이 '지능 쥐' 실험을 할 때마다 ",
	L" ",
	L"뇌파 반응을 보기 위하여 연구실에 ",
	L"K-POP 노래를 계속 틀고 연구를 진행했고,",
	L"그 영향으로 이 '지능 쥐'는 때때로 ",
	L"K-POP 노래에 맞춰 춤을 추는 것이다.",
	L" ",
	L"이 쥐에 물리면, 물린 사람도 좀비화가 진행되며",
	L"가끔 춤을 추고 싶은 욕구에 휘말리게 될 가능성도 있다고 한다.",
};


typedef struct IntroSceneData
{
	Text	Inro1Line[5];
	Text	Inro2Line[8];
	Text	Inro3Line[8];
	Text	Inro4Line[12];
	Text	Inro5Line[10];
	Text	Inro6Line[10];
	int32	FontSize;
	int32	RenderMode;
	Image	Intro1Image;
	int32	X;
	int32	Y;
	Image	Intro2Image;
	Music	IntroBGM;
	float   Volume;

} IntroSceneData;
int count = 1;
void init_intro(void)
{
	count = 1;
	g_Scene.Data = malloc(sizeof(IntroSceneData));
	memset(g_Scene.Data, 0, sizeof(IntroSceneData));

	IntroSceneData* data = (IntroSceneData*)g_Scene.Data;

	for (int32 i = 0; i < 5; ++i)
	{

		Text_CreateText(&data->Inro1Line[i], TextFont, 25, intro1str[i], wcslen(intro1str[i]));
	}
	for (int32 i = 0; i < 8; ++i)
	{
		Text_CreateText(&data->Inro2Line[i], TextFont, 25, intro2str[i], wcslen(intro2str[i]));
	}
	for (int32 i = 0; i < 8; ++i)
	{
		Text_CreateText(&data->Inro3Line[i], TextFont, 25, intro3str[i], wcslen(intro3str[i]));
	}
	for (int32 i = 0; i < 12; ++i)
	{
		Text_CreateText(&data->Inro4Line[i], TextFont, 25, intro4str[i], wcslen(intro4str[i]));
	}
	for (int32 i = 0; i < 10; ++i)
	{
		Text_CreateText(&data->Inro5Line[i], TextFont, 25, intro5str[i], wcslen(intro5str[i]));
	}
	for (int32 i = 0; i < 10; ++i)
	{
		Text_CreateText(&data->Inro6Line[i], TextFont, 25, intro6str[i], wcslen(intro6str[i]));
	}


	data->RenderMode = SOLID;

	Image_LoadImage(&data->Intro1Image, "Intro-1.jpg");
	Image_LoadImage(&data->Intro2Image, "Intro-2.jpg");
	data->X = 0;
	data->Y = 0;

	Audio_LoadMusic(&data->IntroBGM, "HappyLab.mp3");

	Audio_PlayFadeIn(&data->IntroBGM, INFINITY_LOOP, 3000);

	data->Volume = 1.0f;

}
void update_intro(void)
{
	if (Input_GetKeyDown(VK_SPACE))
	{
		count++;
		if (count == 7)
		{
			Scene_SetNextScene(SCENE_MAIN);
		}
	}

}
#define INTRO_X 430
void render_intro(void)
{
	IntroSceneData* data = (IntroSceneData*)g_Scene.Data;
	Renderer_DrawImage(&data->Intro1Image, data->X, data->Y);
	if (count >= 4)
	{
		Renderer_DrawImage(&data->Intro2Image, data->X, data->Y);
	}
	SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 255 };

	if (count == 1)
	{
		for (int32 i = 0; i < 5; ++i)
		{
			//Text_SetFontStyle(&data->Inro1Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro1Line[i], INTRO_X, 220 + (50 * i), color);
		}
	}
	if (count == 2)
	{
		for (int32 i = 0; i < 8; ++i)
		{
			//Text_SetFontStyle(&data->Inro2Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro2Line[i], INTRO_X, 220 + (50 * i), color);
		}
	}
	if (count == 3)
	{
		for (int32 i = 0; i < 8; ++i)
		{
			//Text_SetFontStyle(&data->Inro3Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro3Line[i], INTRO_X, 220 + (50 * i), color);
		}
	}
	if (count == 4)
	{
		for (int32 i = 0; i < 12; ++i)
		{
			//Text_SetFontStyle(&data->Inro4Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro4Line[i], INTRO_X, 220 + (50 * i), color);
		}
	}
	if (count == 5)
	{
		for (int32 i = 0; i < 10; ++i)
		{
			//Text_SetFontStyle(&data->Inro5Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro5Line[i], INTRO_X, 220 + (50 * i), color);
		}
	}
	if (count == 6)
	{
		for (int32 i = 0; i < 10; ++i)
		{
			//Text_SetFontStyle(&data->Inro6Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro6Line[i], INTRO_X, 220 + (50 * i), color);
		}
	}



}

void release_intro(void)
{
	IntroSceneData* data = (IntroSceneData*)g_Scene.Data;
	for (int32 i = 0; i < 5; ++i)
	{
		Text_FreeText(&data->Inro1Line[i]);
	}
	for (int32 i = 0; i < 8; ++i)
	{
		Text_FreeText(&data->Inro2Line[i]);
	}
	for (int32 i = 0; i < 8; ++i)
	{
		Text_FreeText(&data->Inro3Line[i]);
	}
	for (int32 i = 0; i < 12; ++i)
	{
		Text_FreeText(&data->Inro4Line[i]);
	}
	for (int32 i = 0; i < 10; ++i)
	{
		Text_FreeText(&data->Inro5Line[i]);
	}
	for (int32 i = 0; i < 10; ++i)
	{
		Text_FreeText(&data->Inro6Line[i]);
	}

	Audio_FreeMusic(&data->IntroBGM);
	SafeFree(g_Scene.Data);
}
#pragma endregion

#pragma region TempScene
void logOnFinished(void)
{
	LogInfo("You can show this log on stopped the music");
}

void log2OnFinished(int32 channel)
{
	LogInfo("You can show this log on stopped the effect");
}
#pragma endregion

#pragma region MainScene
typedef struct tagMainScene {
	SceneStruct Scene;
	bool        isEndScene;
	int32		CurrentOptionNumber;
	int32		CurrentTextNumber;
	SDL_Color	OptionColors[MAX_OPTION_COUNT];
	Image*		CurrentBGImage;
	int32		BGAlpha;
	Image		BlackOutImage;
	int32		BlackOutAlpha;
	float		ElapsedTime;
	bool isSceneChanging;
	bool showOptions;
	bool isBGChanged;
	bool showItemImage;
	bool isItemPounding;
	bool isItemBigger;
	bool isImagePushing;
	Text* ShowText;
	Text NullText;
	SDL_Color TextColor;
	Image OptionPointImage;
	int32 CurrentBGChangeNumber;
	Image UiImage;
} MainScene;

static int32 s_CurrentScene = 1;
static Music CurrentBGM;
static int32 CurrentBGMNumber = BGM_TITLE;

void ChangeBGM(int32 BGMNum) {
	if (CurrentBGMNumber == BGMNum) {
		return;
	}

	Audio_FreeMusic(&CurrentBGM);
	CurrentBGMNumber = BGMNum;
	switch (BGMNum) {
	case BGM_TITLE:
		Audio_LoadMusic(&CurrentBGM, "title.mp3");
		break;
	case BGM_MA_E2:
		Audio_LoadMusic(&CurrentBGM, "MA_E2YOU_DeadAreComing_Main.mp3");
		break;
	case BGM_IPARK_START:
		Audio_LoadMusic(&CurrentBGM, "ipark_start.mp3");
		break;
	case BGM_GAME_CENTER:
		Audio_LoadMusic(&CurrentBGM, "GAME_CENTER.mp3");
		break;
	case BGM_3ED:
		Audio_LoadMusic(&CurrentBGM, "3.Ed-Lad.in.mp3");
		break;
	case BGM_ENDING_HAPPY:
		Audio_LoadMusic(&CurrentBGM, "Ending.happy.mp3");
		break;
	case BGM_ENDING_SAD:
		Audio_LoadMusic(&CurrentBGM, "Ending.SAD.mp3");
		break;
	case BGM_ZOMBIES:
		Audio_LoadMusic(&CurrentBGM, "zombies are coming.mp3");
		break;
	case BGM_BSIN:
		Audio_LoadMusic(&CurrentBGM, "Bsin.mp3");
		break;
	case BGM_BEGAUN_CUNG:
		Audio_LoadMusic(&CurrentBGM, "Begaun_cungcung.mp3");
		break;
	case BGM_BEGAUN_UNG:
		Audio_LoadMusic(&CurrentBGM, "Begaun_ungsung.mp3");
		break;
	case BGM_DUGENG:
		Audio_LoadMusic(&CurrentBGM, "dugengdugen.mp3");
		break;
	case BGM_KIZUNGUNG:
		Audio_LoadMusic(&CurrentBGM, "kizungung.mp3");
		break;
	case BGM_IPARK_START_DRUM:
		Audio_LoadMusic(&CurrentBGM, "ipark_start_drum.mp3");
		break;
	case BGM_IPARK_START_RAT:
		Audio_LoadMusic(&CurrentBGM, "ipark_start_rat.mp3");
		break;
	case BGM_IPARK_SHEEEIC:
		Audio_LoadMusic(&CurrentBGM, "sheeeeeic.mp3");
		break;
	case BGM_JUNGHU:
		Audio_LoadMusic(&CurrentBGM, "junghu.mp3"); 
	default:
		printf("ERROR!!! WORNG BGM NUMBER\n");
		break;
	}
}

void init_main(void)
{
	g_Scene.Data = malloc(sizeof(MainScene));
	memset(g_Scene.Data, 0, sizeof(MainScene));

	MainScene* data = (MainScene*)g_Scene.Data;


	GetSceneData(s_CurrentScene, &data->Scene);

	//data->Scene = &Scenes[s_CurrentScene];
	data->isEndScene = false;
	data->CurrentOptionNumber = 0;
	//현재 텍스트 번째 수
	data->CurrentTextNumber = 0;
	//암전 효과
	data->BlackOutAlpha = 255;
	Image_LoadImage(&data->BlackOutImage, "black.jpg");
	Image_SetAlphaValue(&data->BlackOutImage, data->BlackOutAlpha);
	//시간 관련
	data->ElapsedTime = 0.0f;
	//BGM 관련
	if (CurrentBGMNumber != data->Scene.BGMNumber) {
		if (Audio_IsMusicPlaying() || Audio_IsMusicFading()) {
			Audio_FadeOut(1800);
		}
		else {
			ChangeBGM(data->Scene.BGMNumber);
			Audio_PlayFadeIn(&CurrentBGM, INFINITY_LOOP, 2000);
		}
	}

	//Audio_PlayFadeIn(&data->Scene->BGM, INFINITY_LOOP, 2000);
	//필요한 NULLText 세팅
	Text_CreateText(&data->NullText, TextFont, 25, L"", 0);
	//선택지 표시 이미지 세팅
	Image_LoadImage(&data->OptionPointImage, "point.png");
	data->OptionPointImage.ScaleX = 0.02f;
	data->OptionPointImage.ScaleY = 0.02f;

	//현재 보여주는 bg 이미지
	data->CurrentBGImage = &data->Scene.BGImage;
	data->BGAlpha = 255;

	Image_LoadImage(&data->UiImage, "UI.jpg");

	//선택지 텍스트 요소 값
	for (int32 i = 0; i < data->Scene.OptionCount;i++) {
		data->OptionColors[i].a = 125;
		data->OptionColors[i].r = 225;
		data->OptionColors[i].g = 225;
		data->OptionColors[i].b = 225;
	}

	data->TextColor.a = 255;
	data->TextColor.r = 255;
	data->TextColor.g = 255;
	data->TextColor.b = 255;

	data->isSceneChanging = false;
	data->showOptions = false;
	data->isBGChanged = false;
	data->showItemImage = false;
	data->isItemPounding = false;
	data->isItemBigger = false;
	data->isImagePushing = false;
	data->ShowText = NULL;
	data->CurrentBGChangeNumber = 0;
}

void update_main(void)
   {
  	MainScene* data = (MainScene*)g_Scene.Data;

	//보통 씬일 경우
	if (!data->isSceneChanging) {
		// 배경 변경 중일 경우
		if (data->isBGChanged) {
			if (data->BGAlpha > 0) {
				if (data->BGAlpha - 25 < 0) {
					data->BGAlpha = 0;
				}
				else {
					data->BGAlpha -= 25;
				}
			}
			else {
				data->isBGChanged = false;
				data->CurrentBGImage = &data->Scene.AdditionBGChangeImages[data->CurrentBGChangeNumber];
				data->ShowText = &data->Scene.DialogList[data->CurrentTextNumber - 1];
				data->TextColor.a = 0;
			}
		}
		// 배경 변경 중일 때(배경의 알파값이 달라짐)
		else if (data->BGAlpha < 255) {
			if (data->BGAlpha + 25 > 255) {
				data->BGAlpha = 255;
			}
			else {
				data->BGAlpha += 25;
			}
		}
		// 씬 최초 불러오기
		else if (data->BlackOutAlpha > 0) {
			data->BlackOutAlpha -= 5;
		}
		//이외 입력 정상 받음
		else {
			//키보드 값 입력
			if (Input_GetKeyDown(VK_SPACE)) {
				//출력 텍스트 조절
				if (data->CurrentTextNumber < data->Scene.DialogCount) {
					data->ShowText = &data->Scene.DialogList[data->CurrentTextNumber];
					data->CurrentTextNumber++;
					data->Scene.ShakingX = 0;
					data->Scene.ShakingY = 0;

					//배경 이미지 변경
					for (int32 i = data->CurrentBGChangeNumber; i < MAX_BG_CHANGE_COUNT;i++) {
						if (data->Scene.AddImageTimings[i] > -1) {
							if (data->CurrentTextNumber == data->Scene.AddImageTimings[i]) {
								data->isBGChanged = true;
								data->ShowText = &data->NullText;
								data->CurrentBGChangeNumber = i;
								data->Scene.ImagePushingX = 0;
								data->Scene.ImagePushingY = 0;
								data->isImagePushing = false;
								break;
							}
						}
						else {
							break;
						}
					}


					//아이템 이미지 적용
					if (data->Scene.AddItemImageTiming > -1) {
						if (!(data->CurrentTextNumber >= data->Scene.AddItemImageTiming && data->CurrentTextNumber < data->Scene.FadeItemImageTiming)) {
							data->showItemImage = false;
						}
						else {
							data->showItemImage = true;
						}
					}

					//배경 밀어내는 효과 적용
					if (data->Scene.ImagePushingTiming > -1) {

						if (data->CurrentTextNumber == data->Scene.ImagePushingTiming) {
							data->isImagePushing = true;
						}
					}

					data->TextColor.a = 0;
				}
				if (data->CurrentTextNumber >= data->Scene.DialogCount && data->Scene.OptionCount <= 0) {
					data->isSceneChanging = true;
					s_CurrentScene = data->Scene.NextSceneNumberList[data->CurrentOptionNumber];
				}
			}
		}

		//텍스트 표시가 필요한 경우
		if (data->CurrentTextNumber < data->Scene.DialogCount) {
			int32 i = 0;
			data->ShowText = data->Scene.DialogList[data->CurrentTextNumber];
			if (data->TextColor.a < 255) {
				data->TextColor.a += 5;
			}
		}
		//옵션이 나와야하는 경우
		else {
			if (data->Scene.OptionCount > 0) {
				data->showOptions = true;
			}

			//선택지 선택
			int32 optionCount = data->Scene.OptionCount;

			//Image_SetAlphaValue(&data->Scene->OptionImagesList[data->CurrentOptionNumber], 125);
			data->OptionColors[data->CurrentOptionNumber].a = 125;
			if (Input_GetKeyDown('1') || Input_GetKeyDown(VK_NUMPAD1)) {
				if (optionCount >= 1) {
					data->CurrentOptionNumber = 0;
				}
			}
			if (Input_GetKeyDown('2') || Input_GetKeyDown(VK_NUMPAD2)) {
				if (optionCount >= 2) {
					data->CurrentOptionNumber = 1;
				}
			}
			if (Input_GetKeyDown('3') || Input_GetKeyDown(VK_NUMPAD3)) {
				if (optionCount >= 3) {
					data->CurrentOptionNumber = 2;
				}
			}
			if (Input_GetKeyDown('4') || Input_GetKeyDown(VK_NUMPAD4)) {
				if (optionCount >= 4) {
					data->CurrentOptionNumber = 3;
				}
			}

			//위 아래 키로
			if (Input_GetKeyDown(VK_UP)) {
				if (data->CurrentOptionNumber > 0) {
					data->CurrentOptionNumber--;
				}
			}

			if (Input_GetKeyDown(VK_DOWN)) {
				if (data->CurrentOptionNumber < data->Scene.OptionCount - 1) {
					data->CurrentOptionNumber++;
				}
			}

			//Image_SetAlphaValue(&data->Scene->OptionImagesList[data->CurrentOptionNumber], 255);
			data->OptionColors[data->CurrentOptionNumber].a = 255;

			//선택지 선택
			if (Input_GetKeyDown(VK_RETURN)) {
				data->isSceneChanging = true;
				s_CurrentScene = data->Scene.NextSceneNumberList[data->CurrentOptionNumber];
			}
		}
	}

	//씬 전환 처리(페이드 인/아웃)
	else {
		if (data->BlackOutAlpha < 255) {
			data->BlackOutAlpha += 5;
		}
		else {
			data->isSceneChanging = false;
			//다음 씬이 -1, 즉 엔딩일때는 타이틀로 돌아감. 아니면 다음 씬을 구성
			if (s_CurrentScene != -2) {
				s_CurrentScene = data->Scene.NextSceneNumberList[data->CurrentOptionNumber];
				Scene_SetNextScene(SCENE_MAIN);
			}
			else {
				data->Scene.isShowedThisEnding = true;
				s_CurrentScene = 0;
				Scene_SetNextScene(SCENE_TITLE);
			}
		}
	}

	//아이템 이미지에 효과 적용(두근두근)
	if (data->showItemImage) {
		data->isItemPounding = true;
		if (data->CurrentTextNumber >= data->Scene.AddPoundingItemImageTiming &&
			data->CurrentTextNumber < data->Scene.FadePoundingItemImageTiming) {
			if (!data->isItemBigger) {
				if (data->Scene.ItemImage.ScaleX < 1.5f) {
					data->Scene.ItemImage.ScaleX += 0.15f;
					data->Scene.ItemImage.ScaleY += 0.15f;
				}
				else {
					data->isItemBigger = true;
				}
			}
			else {
				if (data->Scene.ItemImage.ScaleX > 1.0f) {
					data->Scene.ItemImage.ScaleX -= 0.15f;
					data->Scene.ItemImage.ScaleY -= 0.15f;
				}
				else if (data->Scene.ItemImage.ScaleX <= 1.0f) {
					if (data->ElapsedTime < 0.5f) {
						data->ElapsedTime += Timer_GetDeltaTime();
					}
					else {
						data->ElapsedTime = 0.0f;
						data->isItemBigger = false;
					}
				}
			}
		}
		else {
			data->isItemPounding = false;
			data->Scene.ItemImage.ScaleX = 1.0f;
			data->Scene.ItemImage.ScaleY = 1.0f;
		}
	}

	//이미지 밀어올리기 효과 적용
	if (data->isImagePushing) {
		//특정 씬에서는 좌우로
		if (s_CurrentScene == 1);

		//배경 이동
		if (data->Scene.ImagePushingType == 0) {
			//아직 끝으로 가지 않았을 경우
			if (data->Scene.ImagePushingY - 50 > (data->CurrentBGImage->Height - WINDOW_HEIGHT) * -1) {
				data->Scene.ImagePushingY -= 50;
			}
			//끝에 다다랐을 때
			else {
				data->isImagePushing = false;
			}
		}
		//아이템 이동
		else if (data->Scene.ImagePushingType == 1) {
			//아직 끝으로 가지 않았을 경우
			if (data->Scene.ImagePushingY > (data->Scene.ItemImage.Height + WINDOW_HEIGHT) * -1) {
				data->Scene.ImagePushingY -= 50;
			}
			//끝에 다다랐을 때
			else {
				data->isImagePushing = false;
			}
		}
		//이상한 값
		else {
			printf("ERROR!! WORNG PUSHING TYPE\n");
			data->isImagePushing = false;
		}
	}

	//암전 효과 적용
	Image_SetAlphaValue(&data->BlackOutImage, data->BlackOutAlpha);

	//배경 변경 효과 적용
	Image_SetAlphaValue(data->CurrentBGImage, data->BGAlpha);

}

void render_main(void)
{
	MainScene* data = (MainScene*)g_Scene.Data;

	//배경 이미지 출력
	if (data->Scene.ImagePushingType != 0) {
		Renderer_DrawImage(data->CurrentBGImage, 0, 0);
	}
	else {
		Renderer_DrawImage(data->CurrentBGImage, data->Scene.ImagePushingX, data->Scene.ImagePushingY);
	}

	//아이템 이미지 출력
	if (data->showItemImage) {
		//if (!isItemPounding) {
		//	Renderer_DrawImage(&data->Scene->ItemImage, (WINDOW_WIDTH - data->Scene->ItemImage.Width) / 2, (WINDOW_HEIGHT - data->Scene->ItemImage.Height) / 2 - 100);
		//}
		//else {
		//	Renderer_DrawImage(&data->Scene->ItemImage, 
		//		(WINDOW_WIDTH - data->Scene->ItemImage.Width + data->Scene->ItemImage.Width * (1.0f - data->Scene->ItemImage.ScaleX)) / 2,
		//		(WINDOW_HEIGHT - data->Scene->ItemImage.Height + data->Scene->ItemImage.Height * (1.0f - data->Scene->ItemImage.ScaleY)) / 2 - 100);
		//}

		//두근 거리는 효과
		if (data->isItemPounding) {
			Renderer_DrawImage(&data->Scene.ItemImage,
				(data->Scene.ItemImage.Width * (1.0f - data->Scene.ItemImage.ScaleX)) / 2,
				(data->Scene.ItemImage.Height * (1.0f - data->Scene.ItemImage.ScaleY)) / 2 - 100);
		}
		//이미지 밀어 올리기
		else if (data->Scene.ImagePushingType != 1) {
			Renderer_DrawImage(&data->Scene.ItemImage, 0, 0);
		}
		else {
			Renderer_DrawImage(&data->Scene.ItemImage,
				data->Scene.ImagePushingX, data->Scene.ImagePushingY);
		}
	}

	//UI 출력
	Renderer_DrawImage(&data->UiImage, 0, 0);

	SDL_Color color1 = { 14, 14, 14, 255 };
	//텍스트 출력
	if (data->ShowText != NULL && !data->isBGChanged) {
		int32 i = 0;
		while (data->ShowText[i].Length != 0) {
			Renderer_DrawTextShaded(&data->ShowText[i], 250, 645 + i * 40, data->TextColor, color1);
			i++;
		}
	}

	//선택지 출력
	if (data->showOptions) {
		for (int32 i = 0; i < data->Scene.OptionCount;i++) {
			Renderer_DrawTextShaded(&data->Scene.OptionList[i], 250, 710 + i * 40, data->OptionColors[i], color1);
			Renderer_DrawTextSolid(&data->Scene.OptionList[i], 250, 710 + i * 40, data->OptionColors[i]);
		}
		Renderer_DrawImage(&data->OptionPointImage, 210, 710 + data->CurrentOptionNumber * 40);
	}

	//페이드 인 페이드 아웃 효과를 위한 검은색 배경
	Renderer_DrawImage(&data->BlackOutImage, 0, 0);
}

void release_main(void)
{
	MainScene* data = (MainScene*)g_Scene.Data;

	Scene_Clear(&data->Scene);

	Image_FreeImage(&data->BlackOutImage);
	Text_FreeText(&data->NullText);
	Image_FreeImage(&data->OptionPointImage);
	data->ShowText = 0;
	data->CurrentBGImage = NULL;

	SafeFree(g_Scene.Data);
}

#pragma endregion

bool Scene_IsSetNextScene(void)
{
	if (SCENE_NULL == s_nextScene)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Scene_SetNextScene(ESceneType scene)
{
	assert(s_nextScene == SCENE_NULL);
	assert(SCENE_NULL < scene&& scene < SCENE_MAX);

	s_nextScene = scene;
}

void Scene_Change(void)
{
	assert(s_nextScene != SCENE_NULL);

	if (g_Scene.Release)
	{
		g_Scene.Release();
	}

	switch (s_nextScene)
	{
	case SCENE_TITLE:
		g_Scene.Init = init_title;
		g_Scene.Update = update_title;
		g_Scene.Render = render_title;
		g_Scene.Release = release_title;
		break;
	case SCENE_CREDIT:
		g_Scene.Init = init_credit;
		g_Scene.Update = update_credit;
		g_Scene.Render = render_credit;
		g_Scene.Release = release_credit;
		break;
	case SCENE_INTRO:
		g_Scene.Init = init_intro;
		g_Scene.Update = update_intro;
		g_Scene.Render = render_intro;
		g_Scene.Release = release_intro;
		break;
	case SCENE_MAIN:
		g_Scene.Init = init_main;
		g_Scene.Update = update_main;
		g_Scene.Render = render_main;
		g_Scene.Release = release_main;
		break;
	}

	g_Scene.Init();

	s_nextScene = SCENE_NULL;
}

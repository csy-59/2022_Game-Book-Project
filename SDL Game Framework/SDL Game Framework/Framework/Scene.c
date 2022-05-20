#include "stdafx.h"
#include "Scene.h"

#include "Framework.h"
#include "Csv.h"
#include "Timer.h"
#include "Random.h"

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
	BGM_BPlay,
	BGM_Titel
};

#define SCENE_COUNT	150				//총 씬 갯수(csv 최대값보다 조금 더 많게 설정)
#define MAX_BG_CHANGE_COUNT 1		//배경 이미지 최대로 바뀌는 갯수
#define MAX_TEXT_SET_COUNT 13		//텍스트 세트가 최대로 바뀌는 갯수
#define MAX_TEXT_COUNT 10			//각 텍스트 세트에 최대 줄갯수
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
	int32			ShakingY;									//화면 흔들기에서 사용할 Y값

	int32			DialogCount;									//텍스트 갯수
	Text			DialogList[MAX_TEXT_SET_COUNT][MAX_TEXT_COUNT];	//텍스트 배열

	int32			OptionCount;								//옵션 갯수
	Text			OptionList[MAX_OPTION_COUNT];				//옵션 텍스트 배열

	int32			NextSceneNumberList[MAX_OPTION_COUNT];		//옵션 선택시 넘어가는 씬 넘버

	bool			isShowThisEnding;							//엔딩을 봤는지(엔딩 씬일 경우에만 사용)
} SceneStruct;

SceneStruct Scenes[SCENE_COUNT];
int32 DataCount;

bool isGotData = false;
void GetSceneData(void) {
	CsvFile csv;
	memset(&csv, 0, sizeof(CsvFile));
	CreateCsvFile(&csv, "temp3.csv");

	isGotData = true;

	DataCount = csv.RowCount;

	for (int32 i = 1; i < csv.RowCount;i++) {
		if (i > SCENE_COUNT) {
			break;
		}

		if (csv.Items[i] == NULL) {
			break;
		}

		int32 columCount = 0;

		//씬 번호
		int32 sceneNum = ParseToInt(csv.Items[i][columCount++]) - 1;
		printf("%d\n", sceneNum);
		Scenes[sceneNum].SceneNumber = sceneNum;
		wchar_t* SceneNameTemp = ParseToUnicode(csv.Items[i][columCount++]);
		Text_CreateText(&Scenes[sceneNum].SceneName, TextFont, 25, SceneNameTemp, wcslen(SceneNameTemp));
		Image_LoadImage(&Scenes[sceneNum].BGImage, ParseToAscii(csv.Items[i][columCount++]));
		Audio_LoadMusic(&Scenes[sceneNum].BGM, ParseToAscii(csv.Items[i][columCount++]));

		//배경 전환 이미지
		for (int32 j = 0; j < MAX_BG_CHANGE_COUNT;j++) {
			int32 additionalImagePoint = ParseToInt(csv.Items[i][columCount + 1]);
			Scenes[sceneNum].AddImageTimings[j] = additionalImagePoint;
			if (additionalImagePoint > -1) {
				Image_LoadImage(&Scenes[sceneNum].AdditionBGChangeImages[j], ParseToAscii(csv.Items[i][columCount]));
			}
			columCount += 2;
		}

		//효과음에서 사용했던 것. 나중에 수정할 예정
		columCount+=2;

		//텍스트 데이터 저장
		int32 dialogCount = ParseToInt(csv.Items[i][columCount++]);
		Scenes[sceneNum].DialogCount = dialogCount;
		for (int32 j = 0; j < MAX_TEXT_SET_COUNT;j++) {
			if (dialogCount > j) {
				wchar_t* originalData = ParseToUnicode(csv.Items[i][columCount]);
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
						Text_CreateText(&Scenes[sceneNum].DialogList[j][dialogListIndex], "GmarketSansTTFLight.ttf", 25, lineData, wcslen(lineData));
						dialogListIndex++;
						lineDataIndex = 0;

						if (*(originalData + originalIndex) == '\0') {
							break;
						}
					}
					originalIndex++;
				}

				Text_CreateText(&Scenes[sceneNum].DialogList[j][dialogListIndex], "GmarketSansTTFLight.ttf", 30, L"", wcslen(L""));
			}
			columCount++;
		}

		//아이템 이미지 예비 입력(텍스트가 4개 이상인 씬에만 이미지 넣음)
		if (dialogCount >= 4) {
			Image_LoadImage(&Scenes[sceneNum].ItemImage, "item_prac.jpg");
			Scenes[sceneNum].AddItemImageTiming = 1;
			Scenes[sceneNum].FadeItemImageTiming = 4;

			Scenes[sceneNum].AddPoundingItemImageTiming = 2;
			Scenes[sceneNum].FadePoundingItemImageTiming = 3;
		}
		
		//화면 밀기 임시 입력(텍스트가 1개면 배경 밀기, 4개 이상이면 아이템 이미지 밀기)
		if (dialogCount >= 4) {
			Scenes[sceneNum].ImagePushingType = 1;
			Scenes[sceneNum].ImagePushingTiming = 3;
		}
		else {
			Scenes[sceneNum].ImagePushingType = 0;
			Scenes[sceneNum].ImagePushingTiming = 1;
		}
		Scenes[sceneNum].ImagePushingX = 0;
		Scenes[sceneNum].ImagePushingY = 0;

		//옵션 데이터 저장
		Scenes[sceneNum].OptionCount = ParseToInt(csv.Items[i][columCount++]);

		//만약 선택지가 없으면 다음 씬과 자동으로 연결시켜준다.
		if (Scenes[sceneNum].OptionCount > 0) {
			for (int32 j = 0; j < MAX_OPTION_COUNT;j++) {
				if (Scenes[i - 1].OptionCount > j) {
					wchar_t* temp = ParseToUnicode(csv.Items[i][columCount]);
					Text_CreateText(&Scenes[sceneNum].OptionList[j], "GmarketSansTTFLight.ttf", 20, temp, wcslen(temp));

					columCount++;
					Scenes[sceneNum].NextSceneNumberList[j] = ParseToInt(csv.Items[i][columCount]) - 1;
					columCount++;
				}
			}
		}
		else {
			Scenes[sceneNum].NextSceneNumberList[0] = ParseToInt(csv.Items[i][++columCount]) - 1;
		}

		//bgm 예비 입력(선택지가 있으면 타이틀 bgm이 들린다.)
		if (Scenes[sceneNum].OptionCount > 0) {
			Scenes[sceneNum].BGMNumber = BGM_Titel;
		}
		else {
			Scenes[sceneNum].BGMNumber = BGM_BPlay;
		}

		Scenes[sceneNum].ShakingX = 0;
		Scenes[sceneNum].ShakingY = 0;
		
		
		//shaking 임시 입력(선택지가 있으면 2번째 화면을 흔든다.)
		Scenes[sceneNum].ShakingTimging = -1;
		if (Scenes[sceneNum].OptionCount > 0) 
		{
 			Scenes[sceneNum].ShakingTimging = 2;
		}
		
		Scenes[sceneNum].isShowThisEnding = false;
	}

	FreeCsvFile(&csv);

}


void Scene_Clear(void) {
	if (isGotData) {
		for (int32 i = 0; i < DataCount;i++) {
			//이미지 해제
			Image_FreeImage(&Scenes[i].BGImage);
			if (Scenes[i].AddImageTimings > -1) {
				Image_FreeImage(&Scenes[i].AdditionBGChangeImages);
			}

			//오디오 해제
			//Audio_FreeMusic(&Scenes[i].BGM);
			//if (Scenes[i].EffectSoundTiming > -1) {
			//	Audio_FreeSoundEffect(&Scenes[i].EffectSound);
			//}

			//텍스트 해제
			for (int32 j = 0; j < Scenes[i].DialogCount;j++) {
				int32 k = 0;
				while (Scenes[i].DialogList[j][k].Length > 0) {
					Text_FreeText(&Scenes[i].DialogList[j][k]);
					k++;
				}
				Text_FreeText(&Scenes[i].DialogList[j][k]);
			}
			for (int32 j = 0; j < Scenes[i].OptionCount;j++) {
				Text_FreeText(&Scenes[i].OptionList[j]);
			}
		}
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
	Image	CreditImage;
	int32	X;
	int32	Y;
} CreditSceneData;

void init_credit(void)
{
	//씬 데이터를 미리 받아옴
	if (!isGotData) {
		GetSceneData();
	}

	g_Scene.Data = malloc(sizeof(CreditSceneData));
	memset(g_Scene.Data, 0, sizeof(CreditSceneData));

	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;

	data->FontSize = 48;
	Text_CreateText(&data->EnterText, "chosun.ttf", data->FontSize, L"--- PRESS THE SPACE ---", 24);

	data->RenderMode = SOLID;

	Image_LoadImage(&data->CreditImage, "credit.jpg");
	data->X = 0;
	data->Y = 0;

}

void update_credit(void)
{
	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;

	if (Input_GetKeyDown(VK_SPACE))
	{
		Scene_SetNextScene(SCENE_INTRO);
		Audio_Stop();
	}
}

void render_credit(void)
{
	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;
	Renderer_DrawImage(&data->CreditImage, data->X, data->Y);
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
	SceneStruct* Scene;
	bool        isEndScene;
	int32		CurrentOptionNumber;
	int32		CurrentTextNumber;
	SDL_Color	OptionColors[MAX_OPTION_COUNT];
	Image		BlackOutImage;
	int32		BlackOutAlpha;
	float		ElapsedTime;
	float       ElapsedTimeShaking;
} MainScene;

bool isSceneChanging = false;
bool showOptions = false;
bool isBGChanged = false;
bool showItemImage = false;
bool isItemPounding = false;
bool isItemBigger = false;
bool isShaking = false;
bool isEnter = true;
Text* ShowText;
Text NullText;
int32 s_CurrentScene = 0;
SDL_Color TextColor = { 255,255,255,0 };
Image OptionPointImage;
Music CurrentBGM;
int32 CurrentBGMNumber = -1;
int32 CurrentBGChangeNumber = 0;

void init_main(void)
{
	g_Scene.Data = malloc(sizeof(MainScene));
	memset(g_Scene.Data, 0, sizeof(MainScene));

	MainScene* data = (MainScene*)g_Scene.Data;

	data->Scene = &Scenes[s_CurrentScene];
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
	data->ElapsedTimeShaking = 0.0f;
	//BGM 관련
	if (CurrentBGMNumber != data->Scene->BGMNumber) {
		//ChangeBGM 함수로 묶을 거임
		Audio_FreeMusic(&CurrentBGM);
		switch (data->Scene->BGMNumber) {
		case BGM_BPlay:
			Audio_LoadMusic(&CurrentBGM, "B-play.mp3");
			break;
		case BGM_Titel:
			Audio_LoadMusic(&CurrentBGM, "title.mp3");
			break;
		}
		//CurrentBGM.Music = data->Scene->BGM.Music;
		CurrentBGMNumber = data->Scene->BGMNumber;
		Audio_PlayFadeIn(&CurrentBGM, INFINITY_LOOP, 2000);
	}
	else if (CurrentBGMNumber == -1) {
		CurrentBGMNumber = 0;
		Audio_LoadMusic(&data->Scene->BGM, "B-play.mp3");
	}

	//Audio_PlayFadeIn(&data->Scene->BGM, INFINITY_LOOP, 2000);
	//필요한 NULLText 세팅
	Text_CreateText(&NullText, TextFont, 25, L"", 0);
	//선택지 표시 이미지 세팅
	Image_LoadImage(&OptionPointImage, "point.png");
	OptionPointImage.ScaleX = 0.02f;
	OptionPointImage.ScaleY = 0.02f;

	Audio_SetVolume(0.5f);

	for (int32 i = 0; i < data->Scene->OptionCount;i++) {
		data->OptionColors[i].a = 125;
		data->OptionColors[i].r = 225;
		data->OptionColors[i].g = 225;
		data->OptionColors[i].b = 225;
	}

	//if (data->Scene->EffectSoundTiming > -1) {
	//	Audio_PlaySoundEffect(&data->Scene->EffectSound, 1);
	//}

	isSceneChanging = false;
	showOptions = false;
	isBGChanged = false;
	showItemImage = false;
	isItemPounding = false;
	isItemBigger = false;
	isShaking = false;
	ShowText = NULL;
	CurrentBGChangeNumber = 0;
}

void update_main(void)
{
	MainScene* data = (MainScene*)g_Scene.Data;

	//보통 씬일 경우
	if (!isSceneChanging) {
		// 배경 변경 중일 경우
		if (isBGChanged) {
			if (data->BlackOutAlpha < 255) {
				data->BlackOutAlpha += 5;
			}
			else {
				isBGChanged = false;
				data->Scene->BGImage = data->Scene->AdditionBGChangeImages[CurrentBGChangeNumber];
				ShowText = &data->Scene->DialogList[data->CurrentTextNumber-1];
				TextColor.a = 0;
			}
		}
		// 씬 최초 불러오기
		else if (data->BlackOutAlpha > 0) {
			data->BlackOutAlpha -= 5;
		}
		//이외 입력 정상 받음
		else {
			//키보드 값 입력
			if (Input_GetKeyDown(VK_SPACE) && isEnter == true) {
				//
				data->Scene->ShakingX = 0;
				data->Scene->ShakingY = 0;
				//출력 텍스트 조절
				if (data->CurrentTextNumber < data->Scene->DialogCount) {
					ShowText = &data->Scene->DialogList[data->CurrentTextNumber];
					data->CurrentTextNumber++;

					//배경 이미지 변경
					for (int32 i = CurrentBGChangeNumber; i < MAX_BG_CHANGE_COUNT;i++) {
						if (data->Scene->AddImageTimings[i] > -1) {
							if (data->CurrentTextNumber == data->Scene->AddImageTimings[i]) {
								isBGChanged = true;
								ShowText = &NullText;
								CurrentBGChangeNumber = i;
								break;
							}
						}
						else {
							break;
						}
					}
					

					//아이템 이미지 적용
					if (data->Scene->AddItemImageTiming > -1) {
						if (!(data->CurrentTextNumber >= data->Scene->AddItemImageTiming && data->CurrentTextNumber < data->Scene->FadeItemImageTiming)) {
							showItemImage = false;
						}
						else {
							showItemImage = true;
						}
					}

					//이펙트 싸운드 적용
					//f (data->Scene->EffectSoundTiming > -1) {
					//	if (data->CurrentOptionNumber == data->Scene->EffectSoundTiming) {
					//		Audio_SetEffectVolume(&data->Scene->EffectSound, 1.0f);
					//		Audio_PlaySoundEffect(&data->Scene->EffectSound, 1);
					//	}
					//

					TextColor.a = 0;
				}
				if (data->CurrentTextNumber >= data->Scene->DialogCount - 1 && data->Scene->OptionCount <= 0) {
					isSceneChanging = true;
					s_CurrentScene = data->Scene->NextSceneNumberList[data->CurrentOptionNumber];
					if (CurrentBGMNumber != Scenes[s_CurrentScene].BGMNumber) {
						Audio_FadeOut(1800);
					}
				}
			}
		}

		//텍스트 표시가 필요한 경우
		if (data->CurrentTextNumber < data->Scene->DialogCount) {
			int32 i = 0;
			ShowText = data->Scene->DialogList[data->CurrentTextNumber];
			if (TextColor.a < 255) {
				TextColor.a += 5;
			}
		}
		//옵션이 나와야하는 경우
		else {
			if (data->Scene->OptionCount > 0) {
				showOptions = true;
			}

			//선택지 선택
			int32 optionCount = data->Scene->OptionCount;

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
				if (data->CurrentOptionNumber < data->Scene->OptionCount - 1) {
					data->CurrentOptionNumber++;
				}
			}

			//Image_SetAlphaValue(&data->Scene->OptionImagesList[data->CurrentOptionNumber], 255);
			data->OptionColors[data->CurrentOptionNumber].a = 255;

			//선택지 선택
			if (Input_GetKeyDown(VK_RETURN)) {
				isSceneChanging = true;
				s_CurrentScene = data->Scene->NextSceneNumberList[data->CurrentOptionNumber];
				if (CurrentBGMNumber != Scenes[s_CurrentScene].BGMNumber) {
					Audio_FadeOut(1800);
				}
				Audio_FadeOutSoundEffect(1800);				
			}
		}
	}

	else {
		if (data->BlackOutAlpha < 255) {
			data->BlackOutAlpha += 5;
		}
		else {
			isSceneChanging = false;
			s_CurrentScene = data->Scene->NextSceneNumberList[data->CurrentOptionNumber];
			//다음 씬이 -1, 즉 엔딩일때는 타이틀로 돌아감. 아니면 다음 씬을 구성
			if (s_CurrentScene != -2) {
				Scene_SetNextScene(SCENE_MAIN);
			}
			else {
				s_CurrentScene = 0;
				Scene_SetNextScene(SCENE_TITLE);
			}
		}
	}

	//아이템 이미지에 효과 적용(두근두근)
	if (showItemImage ) {
		isItemPounding = true;
		if (data->CurrentTextNumber >= data->Scene->AddPoundingItemImageTiming && 
			data->CurrentTextNumber < data->Scene->FadePoundingItemImageTiming) {
			if (!isItemBigger) {
				if (data->Scene->ItemImage.ScaleX < 1.5f) {
					data->Scene->ItemImage.ScaleX += 0.15f;
					data->Scene->ItemImage.ScaleY += 0.15f;
				}
				else {
					isItemBigger = true;
				}
			}
			else {
				if (data->Scene->ItemImage.ScaleX > 1.0f) {
					data->Scene->ItemImage.ScaleX -= 0.15f;
					data->Scene->ItemImage.ScaleY -= 0.15f;
				}
				else if (data->Scene->ItemImage.ScaleX <= 1.0f) {
					if (data->ElapsedTime < 0.5f) {
						data->ElapsedTime += Timer_GetDeltaTime();
					}
					else {
						data->ElapsedTime = 0.0f;
						isItemBigger = false;
					}
				}
			}
		}
		
	}
	//이미지가 흔들리는 효과

		if (data->Scene->ShakingTimging == data->CurrentTextNumber && data->BlackOutAlpha == 0)
		{
			if (!isShaking)
			{
				data->ElapsedTimeShaking += Timer_GetDeltaTime();
				if (data->ElapsedTimeShaking < 10.0f && data->ElapsedTimeShaking > 0.05f)
				{
					data->Scene->ShakingX = Random_GetNumberFromRange(-100, 100);
					data->Scene->ShakingY = Random_GetNumberFromRange(-100, 100);
					isEnter = false;
				}
				else if (data->ElapsedTimeShaking >= 10.0f)
				{
					data->ElapsedTimeShaking = 0.0f;

					data->Scene->ShakingX = 0;
					data->Scene->ShakingY = 0;
					isShaking = true;
					isEnter = true;
				}
			}
		}
	




	//암전 효과 적용
	Image_SetAlphaValue(&data->BlackOutImage, data->BlackOutAlpha);
}
int TEXT_X = 250;

void render_main(void)
{
	MainScene* data = (MainScene*)g_Scene.Data;

	//배경 이미지 출력
	Renderer_DrawImage(&data->Scene->BGImage, 0 + data->Scene->ShakingX, 0 + data->Scene->ShakingY);
	//Renderer_DrawImage(&TextBGImage, 30, 30);

	//아이템 이미지 출력
	if (showItemImage) {
		//if (!isItemPounding) {
		//	Renderer_DrawImage(&data->Scene->ItemImage, (WINDOW_WIDTH - data->Scene->ItemImage.Width) / 2, (WINDOW_HEIGHT - data->Scene->ItemImage.Height) / 2 - 100);
		//}
		//else {
		//	Renderer_DrawImage(&data->Scene->ItemImage, 
		//		(WINDOW_WIDTH - data->Scene->ItemImage.Width + data->Scene->ItemImage.Width * (1.0f - data->Scene->ItemImage.ScaleX)) / 2,
		//		(WINDOW_HEIGHT - data->Scene->ItemImage.Height + data->Scene->ItemImage.Height * (1.0f - data->Scene->ItemImage.ScaleY)) / 2 - 100);
		//}
		if (!isItemPounding) {
			Renderer_DrawImage(&data->Scene->ItemImage, 0 + data->Scene->ShakingX, 0 + data->Scene->ShakingY);
		}
		else {
			Renderer_DrawImage(&data->Scene->ItemImage, 
				(data->Scene->ItemImage.Width * (1.0f - data->Scene->ItemImage.ScaleX)) / 2,
				(data->Scene->ItemImage.Height * (1.0f - data->Scene->ItemImage.ScaleY)) / 2 - 100);
		}
	}

	SDL_Color color1 = { 14, 14, 14, 255 };
	//텍스트 출력
	if (ShowText != NULL && !isBGChanged) {
		int32 i = 0;
		while (ShowText[i].Length != 0) {
			//Renderer_DrawTextBlended(&ShowText[i], 250, 830 + i * 40, TextColor);
			Renderer_DrawTextShaded(&ShowText[i], 250 + data->Scene->ShakingX, 830 + i * 40 + data->Scene->ShakingY, TextColor, color1);
			//Renderer_DrawTextSolid(&ShowText[i], 250, 830 + i * 40, TextColor);
			i++;
		}
	}

	//선택지 출력
	if (showOptions) {
		for (int32 i = 0; i < data->Scene->OptionCount;i++) {
			Renderer_DrawTextShaded(&data->Scene->OptionList[i], 250, 895 + i * 40, data->OptionColors[i], color1);
			Renderer_DrawTextSolid(&data->Scene->OptionList[i], 250, 895 + i * 40, data->OptionColors[i]);
		}
		Renderer_DrawImage(&OptionPointImage, 210 + data->Scene->ShakingX, 895 + data->CurrentOptionNumber * 40 + data->Scene->ShakingY);
	}

	//페이드 인 페이드 아웃 효과를 위한 검은색 배경
	Renderer_DrawImage(&data->BlackOutImage, 0, 0);
}

void release_main(void)
{
	MainScene* data = (MainScene*)g_Scene.Data;

	Image_FreeImage(&data->BlackOutImage);
	//Text_FreeText(&NullText);
	Image_FreeImage(&OptionPointImage);
	ShowText = 0;

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

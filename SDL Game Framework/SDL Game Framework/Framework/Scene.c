#include "stdafx.h"
#include "Scene.h"

#include "Framework.h"
#include "Csv.h"
#include "Timer.h"

Scene g_Scene;

static ESceneType s_nextScene = SCENE_NULL;

#define TITLE_MUSIC "title.mp3"
#define CSV_FILE_NAME "csv_ver4 (5)_temp.csv"
#define ARROW_IMAGE_FILE "point.png"
#define SRINJ_IMAGE_FILE "srinj1.png"

Music   TitleBGM;

#pragma region TitleScene
typedef enum TitleMenu {
	START,
	ENDING,
	EXIT
} titleMenu;

typedef struct TitleSceneData
{
	Text	LoadingText;	
	int32   FontSize;

	Image   TitleImage;

	Image   SrinjImage;
	int32	SX;
	int32	SY;

	Image	StartImage;
	Image	EndingImage;

	Image   BlackOutImage;
	int32   BlackOutAlpha;

} TitleSceneData;

bool Loading = false;
titleMenu title = START;
void init_title(void)
{
	g_Scene.Data = malloc(sizeof(TitleSceneData));
	memset(g_Scene.Data, 0, sizeof(TitleSceneData));

	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;

	data->FontSize = 48;
	Text_CreateText(&data->LoadingText, "GmarketSansTTFBold.ttf", data->FontSize, L"----- LOADING -----", 24);

	Image_LoadImage(&data->TitleImage, "title_2.jpg");

	Image_LoadImage(&data->SrinjImage, SRINJ_IMAGE_FILE);
	data->SX = 550;
	data->SY = 750;
	Image_LoadImage(&data->StartImage, "start_button.png");
	Image_LoadImage(&data->EndingImage, "ending_button.png");

	Audio_LoadMusic(&TitleBGM, TITLE_MUSIC);

	Audio_PlayFadeIn(&TitleBGM, INFINITY_LOOP, 2000);

	data->BlackOutAlpha = 255;
	Image_LoadImage(&data->BlackOutImage, "black.jpg");
	Image_SetAlphaValue(&data->BlackOutImage, data->BlackOutAlpha);

	title = START;

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
		if (Input_GetKeyDown(VK_UP))
		{
			title = START;
			data->SX = 550;
			data->SY = 750;

		}
		if (Input_GetKeyDown(VK_DOWN))
		{
			title = ENDING;
			data->SX = 550;
			data->SY = 800;
		}
		if (Input_GetKeyDown(VK_RETURN))
		{
			if (title == START)
			{
				Loading = true;
				Scene_SetNextScene(SCENE_CREDIT);
			}
			else if (title == ENDING)
			{

				Loading = true;
				Scene_SetNextScene(SCENE_END);

			}
		}

	}
	Image_SetAlphaValue(&data->BlackOutImage, data->BlackOutAlpha);

	if (Input_GetKeyDown(VK_ESCAPE)) {
		s_IsGameClose = true;
	}
}
void render_title(void)
{
	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;
	Renderer_DrawImage(&data->TitleImage, 0, 0);
	static float elapsedTime = 1.0f;

	SDL_Color color = { .a = 255 };
	static bool isShow = true;
	elapsedTime += Timer_GetDeltaTime();
	if (!Loading)
	{
		if (elapsedTime >= 1.0f)
		{
			if (elapsedTime >= 1.5f)
			{
				elapsedTime = 0.0f;
			}
		}
		Renderer_DrawImage(&data->StartImage, (WINDOW_WIDTH / 2) - (data->StartImage.Width * data->StartImage.ScaleX / 2), 750);
		Renderer_DrawImage(&data->EndingImage, (WINDOW_WIDTH / 2) - (data->EndingImage.Width * data->EndingImage.ScaleX / 2), 800);
		Renderer_DrawImage(&data->SrinjImage, data->SX, data->SY);
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

	Text_FreeText(&data->LoadingText);
	Image_FreeImage(&data->TitleImage);
	Image_FreeImage(&data->SrinjImage);
	Image_FreeImage(&data->StartImage);
	Image_FreeImage(&data->BlackOutImage);
	Image_FreeImage(&data->EndingImage);


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
	BGM_KIZUNGUNG = 12,
	BGM_IPARK_START_DRUM = 13,
	BGM_SHEEEIC = 15,
	BGM_JUNGHU = 16,
};

#define MAX_BG_CHANGE_COUNT 4		//배경 이미지 최대로 바뀌는 갯수
#define MAX_EFFECT_SOUND_COUNT 3	//이펙스 사운드 갯수
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

	SoundEffect		SoundEffects[MAX_EFFECT_SOUND_COUNT];		//사운드 이펙트 배열
	int32			AddSoundEffectTimings[MAX_EFFECT_SOUND_COUNT];//사운드 이펙트 타이밍

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

	bool			IsNextSceneBGMChange[MAX_OPTION_COUNT];			//다음 씬에서 BGMNUM이 바뀌는 확인
} SceneStruct;

void GetSceneData(int32 sceneNum, SceneStruct* scene) {
	CsvFile csv;
	memset(&csv, 0, sizeof(CsvFile));
 	CreateCsvFile(&csv, CSV_FILE_NAME);

	//이상한 씬넘버가 들어오면 나가기
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

	//이펙트 사운드
	for (int32 j = 0; j < MAX_EFFECT_SOUND_COUNT; j++) {
		int32 addEffectSoundPoint = ParseToInt(csv.Items[sceneNum][columCount + 1]);
 		scene->AddSoundEffectTimings[j] = addEffectSoundPoint;
		if (addEffectSoundPoint > -1) {
			Audio_LoadSoundEffect(&scene->SoundEffects[j], ParseToAscii(csv.Items[sceneNum][columCount]));
		}
		columCount += 2;
	}

	//아이템 이미지
	int32 itemImagePoint = ParseToInt(csv.Items[sceneNum][columCount + 1]);
	scene->AddItemImageTiming = itemImagePoint;
	printf("%d\n", scene->AddItemImageTiming);
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
	scene->ImagePushingType = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->ImagePushingTiming = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->ImagePushingX = 0;
	scene->ImagePushingY = 0;

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
				//다음씬에서 bgm이 바뀌는지 확인하는 것
				if (scene->NextSceneNumberList[j] > -1) {
					scene->IsNextSceneBGMChange[j] =
						ParseToInt(csv.Items[scene->NextSceneNumberList[j]][3]) != scene->BGMNumber;
				}
				columCount++;
			}
		}
	}
	else {
		scene->NextSceneNumberList[0] = ParseToInt(csv.Items[sceneNum][++columCount]);
		if (scene->NextSceneNumberList[0] > -1) {
			scene->IsNextSceneBGMChange[0] =
				ParseToInt(csv.Items[scene->NextSceneNumberList[0]][3]) != scene->BGMNumber;
		}
	}

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
	//Audio_FreeMusic(&scene->BGM);
	for (int32 j = 0; j < MAX_EFFECT_SOUND_COUNT; j++) {
		if (scene->AddSoundEffectTimings[j] > -1) {
			Audio_FreeSoundEffect(&scene->SoundEffects[j]);
		}
		else {
			break;
		}
	}

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

	if (Input_GetKeyDown(VK_ESCAPE)) {
		s_IsGameClose = true;
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
	Audio_FreeMusic(&TitleBGM);

	Image_FreeImage(&data->CreditImage[0]);
	Image_FreeImage(&data->CreditImage[1]);
	Image_FreeImage(&data->CreditImage[2]);


	SafeFree(g_Scene.Data);
}
#pragma endregion

#pragma region Logs

void logOnFinished(void)
{
	LogInfo("You can show this log on stopped the music");
}

void log2OnFinished(int32 channel)
{
	LogInfo("You can show this log on stopped the effect");
}
#pragma endregion

bool s_IsEndingScene = false;		//엔딩 모음집과 연결되었는지 확인
static int32 s_CurrentScene = 54;	//현재 씬 넘버

#pragma region MainScene
typedef struct tagMainScene {
	SceneStruct Scene;								//씬 정보			

	int32		CurrentOptionNumber;				//현재 선택중인 선택지 번호
	SDL_Color	OptionColors[MAX_OPTION_COUNT];		//선택지 마다의 색 표현. 이걸 활용하여 투명도를 조절한다.

	int32		CurrentTextNumber;					//현재 보여주고 있는 텍스트 번호

	Image*		CurrentBGImage;						//현재 보여주고 있는 배경 이미지. 다른것이 아닌 이것만 랜더 해준다.
	int32		BGAlpha;							//배경화면의 투명도. 배경 전환시 사용

	Image		BlackOutImage;						//씬 이동시 페이드 인/아웃에 적용할 검은 배경
	int32		BlackOutAlpha;						//검은 배경의 투명도
	
	float		ElapsedTime;						//두근두근 효과에 사용할 시간
	float		ElapsedShakingTime;					//화면 흔들림 효과에 적용할 시간

	bool isSceneChanging;
	bool showOptions;
	bool isBGChanged;
	bool showItemImage;
	bool isItemPounding;
	bool isItemBigger;
	bool isImagePushing;
	bool isShaking;
	bool isEnter;
	bool isShowingPopUp;
	Text* ShowText;
	Text NullText;
	SDL_Color TextColor;
	Image OptionPointImage;
	int32 CurrentBGChangeNumber;
	int32 CurrentSoundEffectNumber;
	Image UiImage;
	Image PopupImage;
} MainScene;

static int32 SCROLE_SPEED = 20;

static Music CurrentBGM;
static int32 CurrentBGMNumber = BGM_TITLE;

void ChangeBGM(int32 BGMNum) {
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
	case BGM_KIZUNGUNG:
		Audio_LoadMusic(&CurrentBGM, "kizungung.mp3");
		break;
	case BGM_IPARK_START_DRUM:
		Audio_LoadMusic(&CurrentBGM, "ipark_start_drum.mp3");
		break;
	case BGM_SHEEEIC:
		Audio_LoadMusic(&CurrentBGM, "sheeeeeic.mp3");
		break;
	case BGM_JUNGHU:
		Audio_LoadMusic(&CurrentBGM, "junghu.mp3");
		break;
	case -1:
		Audio_LoadMusic(&CurrentBGM, "NoMusic.mp3");
		break;
	default:
		printf("ERROR!!! WORNG BGM NUMBER\n");
		break;
	}
}

bool IsThisEndingShown(int32 SceneNum) {
	FILE* fp = NULL;

	//우선 파일을 읽어서 데이터를 받아 어디에 저장할지 판별한다.
	errno_t err = fopen_s(&fp, "Asset/Data/endingLog.txt", "r");
	
	if (err != 0) {
		printf("ERROR!!! CAN'T READ endingLog.txt\n");
		return;
	}

	char endNum[128] = "";
	while (fgets(endNum, sizeof(endNum), fp) != NULL) {
		int32 num = atoi(endNum);
		printf("%d\n", num);
		if (num == SceneNum) {
			fclose(fp);
			return true;
		}
		else if (num > SceneNum) {
			fclose(fp);
			return false;
		}
		endNum[0] = '\n';
	}
	fclose(fp);

	return false;
}
void RecordThisEnding(int32 SceneNum) {

	FILE* fp = NULL;

	fopen_s(&fp, "Asset/Data/endingLog.txt", "r");
	if (fp == NULL) {
		printf("ERROR!!! CAN'T READ endingLog.txt\n");
		return;
	}

	//for (int32 i = 0; i < 17; i++) {
	//	IsThisEndingShown(i + 121);
	//	printf("\n");
	//}

	bool endingList[17] = { false };
	endingList[SceneNum - 121] = true;
	char endNum[128] = "";

	while (fgets(endNum, sizeof(endNum), fp) != NULL) {
		if (strlen(endNum) != 0) {
			int32 num = atoi(endNum);
			endingList[num - 121] = true;
			endNum[0] = '\n';
		}
		else {
			break;
		}

	}
	fclose(fp);

	//해당 데이터를 기반으로 데이터 다시 저장
	fopen_s(&fp, "Asset/Data/endingLog.txt", "w");
	if (fp == NULL) {
		printf("ERROR!!! CAN'T READ endingLog.txt\n");
		return;
	}

	endNum[0] = '\0';
	for (int32 i = 0; i < 17; i++) {
		if (endingList[i]) {
			char tempNum[10];
			_itoa_s(i + 121, tempNum, sizeof(tempNum), 10);
			strcat_s(endNum, sizeof(endNum), tempNum);
			strcat_s(endNum, sizeof(endNum), "\n");
		}
	}
	fputs(endNum, fp);
	fclose(fp);
}

void init_main(void)
{
	g_Scene.Data = malloc(sizeof(MainScene));
	memset(g_Scene.Data, 0, sizeof(MainScene));

	MainScene* data = (MainScene*)g_Scene.Data;

	GetSceneData(s_CurrentScene, &data->Scene);

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
		ChangeBGM(data->Scene.BGMNumber);
		Audio_PlayFadeIn(&CurrentBGM, INFINITY_LOOP, 2000);
	}
	else if (!Audio_IsMusicPlaying() || Audio_IsMusicFading()) {
		ChangeBGM(data->Scene.BGMNumber);
		Audio_PlayFadeIn(&CurrentBGM, INFINITY_LOOP, 2000);
	}

	printf("%d\n", CurrentBGMNumber);

	//Audio_PlayFadeIn(&data->Scene->BGM, INFINITY_LOOP, 2000);
	//필요한 NULLText 세팅
	Text_CreateText(&data->NullText, TextFont, 25, L"", 0);
	//선택지 표시 이미지 세팅
	Image_LoadImage(&data->OptionPointImage, SRINJ_IMAGE_FILE);
	data->OptionPointImage.ScaleX = 0.35f;
	data->OptionPointImage.ScaleY = 0.35f;

	//현재 보여주는 bg 이미지
	data->CurrentBGImage = &data->Scene.BGImage;
	data->BGAlpha = 255;

	//UI 이미지
	Image_LoadImage(&data->UiImage, "UI.jpg");

	//팝업 이미지
	Image_LoadImage(&data->PopupImage, "PopUp.png");

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

	data->ElapsedTime = 0.0f;
	data->ElapsedShakingTime = 0.0f;

	data->isSceneChanging = false;
	data->showOptions = false;
	data->isBGChanged = false;
	data->showItemImage = false;
	data->isItemPounding = false;
	data->isItemBigger = false;
	data->isImagePushing = false;
	data->isShaking = false;
	data->isEnter = false;
	data->isShowingPopUp = false;
	data->ShowText = NULL;
	data->CurrentBGChangeNumber = 0;
	data->CurrentSoundEffectNumber = 0;

	//엔딩 씬이라면 파일에 저장
	if (s_CurrentScene >= 121 && s_CurrentScene < 138) {
		RecordThisEnding(data->Scene.SceneNumber);
	}

	if (s_CurrentScene == 138) {
		SCROLE_SPEED = 3;
	}
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
				data->ShowText = &data->Scene.DialogList[data->CurrentTextNumber];
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
			if (!data->isShowingPopUp) {
				data->BlackOutAlpha -= 5;
			}
		}
		//이외 입력 정상 받음
		else {
			//키보드 값 입력
			if (Input_GetKeyDown(VK_SPACE)) {
				//출력 텍스트 조절
				if (data->CurrentTextNumber < data->Scene.DialogCount) {
					data->ShowText = &data->Scene.DialogList[data->CurrentTextNumber - 1];
					data->CurrentTextNumber++;
					Audio_FadeOutSoundEffect(800);
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

					//이펙트 사운드 추가
					for (int32 i = data->CurrentSoundEffectNumber; i < MAX_EFFECT_SOUND_COUNT; i++) {
						if (data->Scene.AddSoundEffectTimings[i] > -1) {
							if (data->CurrentTextNumber == data->Scene.AddSoundEffectTimings[i]) {
								Audio_PlaySoundEffect(&data->Scene.SoundEffects[i], 0);
								data->CurrentBGChangeNumber = i;
								break;
							}
						}
						else {
							break;
						}
					}

					data->TextColor.a = 0;
				}
				if (data->CurrentTextNumber >= data->Scene.DialogCount && data->Scene.OptionCount <= 0) {
					data->isSceneChanging = true;
 					if (data->Scene.IsNextSceneBGMChange[0]) {
						Audio_FadeOut(1800);
						Audio_FadeOutSoundEffect(1800);
					}
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

			//위 아래 키로 선택지 선택
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
			if (Input_GetKeyDown(VK_RETURN) && !data->isShowingPopUp) {
				data->isSceneChanging = true;
				s_CurrentScene = data->Scene.NextSceneNumberList[data->CurrentOptionNumber];
				if (data->Scene.IsNextSceneBGMChange[data->CurrentOptionNumber]) {
					Audio_FadeOut(1800);
					Audio_FadeOutSoundEffect(1800);
				}
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
			if (s_CurrentScene != -1 && !data->isShowingPopUp) {
				//s_CurrentScene = data->Scene.NextSceneNumberList[data->CurrentOptionNumber];
				Scene_SetNextScene(SCENE_MAIN);
			}
			else if (s_IsEndingScene && !data->isShowingPopUp) {
				s_IsEndingScene = false;
				Scene_SetNextScene(SCENE_END);
			}
			else {
				//data->Scene.isShowedThisEnding = true;
				//엔딩을 봤음을 기록
				s_CurrentScene = 1;
				Scene_SetNextScene(SCENE_TITLE);
			}
			data->isShowingPopUp = false;
		}
	}

	//타이틀로 가는 esc 팝업 화면
	if (Input_GetKeyDown(VK_ESCAPE)) {
		if (!data->isShowingPopUp) {
			data->isShowingPopUp = true;
			Audio_Pause();
			Audio_StopSoundEffect();
			data->BlackOutAlpha = 125;
		}
		else {
			data->isShowingPopUp = false;
			Audio_Resume();
			Audio_ResumeSoundEffect();
		}
	}

	//팝업
	if (data->isShowingPopUp) {
		if (Input_GetKeyDown(VK_RETURN)) {
			data->isSceneChanging = true;
			s_CurrentScene = -1;
		}
	}

	//팝업 표현 중일 때는 효과 적용X
	if (!data->isShowingPopUp) {
		//아이템 이미지 적용
		if (data->CurrentTextNumber >= data->Scene.AddItemImageTiming && data->CurrentTextNumber < data->Scene.FadeItemImageTiming) {
			data->showItemImage = true;
		}
		else {
			data->showItemImage = false;
		}

		//배경 밀어내는 효과 적용
		if (data->CurrentTextNumber == data->Scene.ImagePushingTiming) {

			//배경 이동
			if (data->Scene.ImagePushingType == 0) {
				//아직 끝으로 가지 않았을 경우
				if (data->Scene.ImagePushingY - SCROLE_SPEED > (data->CurrentBGImage->Height - WINDOW_HEIGHT) * -1) {
					data->Scene.ImagePushingY -= SCROLE_SPEED;
				}
				//끝에 다다랐을 때
				else {
					data->isImagePushing = false;
				}
			}
			//아이템 이동
			else if (data->Scene.ImagePushingType == 1) {
				////특정 씬에서는 좌우로
				//if (s_CurrentScene != 107) {
				//	//아직 끝으로 가지 않았을 경우
				//	if (data->Scene.ImagePushingY > (data->Scene.ItemImage.Height) * -1) {
				//		data->Scene.ImagePushingY -= SCROLE_SPEED;
				//	}
				//	//끝에 다다랐을 때
				//	else {
				//		data->isImagePushing = false;
				//	}
				//}
				//else {
				//	

				//}

				//아직 끝으로 가지 않았을 경우
				if (data->Scene.ImagePushingX > (data->Scene.ItemImage.Width) * -1) {
					data->Scene.ImagePushingX -= SCROLE_SPEED;
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

		//아이템 이미지에 효과 적용(두근두근)
		if (data->CurrentTextNumber >= data->Scene.AddPoundingItemImageTiming &&
			data->CurrentTextNumber < data->Scene.FadePoundingItemImageTiming) {
			data->isItemPounding = true;
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


		//화면 흔들리는 효과
		if (data->Scene.ShakingTimging == data->CurrentTextNumber)
		{
			if (!data->isShaking)
			{
				data->ElapsedShakingTime += Timer_GetDeltaTime();

				float shakingTime = 0.5f;
				if (!data->BlackOutAlpha == 0) {
					shakingTime = 0.7f;
				}

				if (data->ElapsedShakingTime < shakingTime && data->ElapsedShakingTime > 0.05f)
				{
					data->Scene.ShakingX = Random_GetNumberFromRange(-100, 100);
					data->Scene.ShakingY = Random_GetNumberFromRange(-100, 100);
					data->isEnter = false;
				}
				else if (data->ElapsedShakingTime >= shakingTime)
				{
					data->ElapsedShakingTime = 0.0f;

					data->Scene.ShakingX = 0;
					data->Scene.ShakingY = 0;
					data->isShaking = true;
					data->isEnter = true;
				}
			}
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
		Renderer_DrawImage(data->CurrentBGImage, data->Scene.ShakingX, data->Scene.ShakingY);
	}
	else {
		Renderer_DrawImage(data->CurrentBGImage, 
			data->Scene.ImagePushingX + data->Scene.ShakingX,
			data->Scene.ImagePushingY + data->Scene.ShakingY);
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
				(data->Scene.ItemImage.Width * (1.0f - data->Scene.ItemImage.ScaleX)) / 2 + data->Scene.ShakingX,
				(data->Scene.ItemImage.Height * (1.0f - data->Scene.ItemImage.ScaleY)) / 2 + data->Scene.ShakingY);
		}
		else if (data->Scene.ImagePushingType != 1) {
			Renderer_DrawImage(&data->Scene.ItemImage, data->Scene.ShakingX, data->Scene.ShakingY);
		}
		//이미지 밀어 올리기
		else {
			Renderer_DrawImage(&data->Scene.ItemImage,
				data->Scene.ImagePushingX, data->Scene.ImagePushingY);
		}
	}

	//UI 출력
	if (s_CurrentScene < 121) {
		Renderer_DrawImage(&data->UiImage, data->Scene.ShakingX, data->Scene.ShakingY);
	}

	int32 finalTextPosY = 0;
	SDL_Color color1 = { 14, 14, 14, 255 };
	//텍스트 출력
	if (data->ShowText != NULL && !data->isBGChanged) {
		int32 i = 0;
		while (data->ShowText[i].Length != 0) {
			Renderer_DrawTextShaded(&data->ShowText[i], 250 + data->Scene.ShakingX, 645 + i * 40 + data->Scene.ShakingY, data->TextColor, color1);
			finalTextPosY = 645 + i * 40 + data->Scene.ShakingY;
			i++;
		}
	}

	//선택지 출력
	if (finalTextPosY == 0) {
		finalTextPosY = 645;
	}
	if (data->showOptions) {
		for (int32 i = 0; i < data->Scene.OptionCount;i++) {
			Renderer_DrawTextShaded(&data->Scene.OptionList[i], 250 + data->Scene.ShakingX, finalTextPosY + 65 + i * 40 + data->Scene.ShakingY, data->OptionColors[i], color1);
			Renderer_DrawTextSolid(&data->Scene.OptionList[i], 250 + data->Scene.ShakingX, finalTextPosY + 65 + i * 40 + data->Scene.ShakingY, data->OptionColors[i]);
		}
		Renderer_DrawImage(&data->OptionPointImage, 200 + data->Scene.ShakingX, finalTextPosY + 68 + data->CurrentOptionNumber * 40 + data->Scene.ShakingY);
	}

	if (!data->isShowingPopUp) {
		//페이드 인 페이드 아웃 효과를 위한 검은색 배경
		Renderer_DrawImage(&data->BlackOutImage, data->Scene.ShakingX, data->Scene.ShakingY);
	}
	else if (!data->isSceneChanging) {
		Renderer_DrawImage(&data->BlackOutImage, data->Scene.ShakingX, data->Scene.ShakingY);
		Renderer_DrawImage(&data->PopupImage, 0, 0);
	}
	else {
		Renderer_DrawImage(&data->PopupImage, 0, 0);
		Renderer_DrawImage(&data->BlackOutImage, data->Scene.ShakingX, data->Scene.ShakingY);
	}

}

void release_main(void)
{
	MainScene* data = (MainScene*)g_Scene.Data;

	Scene_Clear(&data->Scene);
	data->CurrentBGImage = NULL;
	Image_FreeImage(&data->BlackOutImage);
	Image_FreeImage(&data->OptionPointImage);
	data->ShowText = NULL;
	Text_FreeText(&data->NullText);
	Image_FreeImage(&data->UiImage);
	Image_FreeImage(&data->PopupImage);
	data->ShowText = 0;

	SafeFree(g_Scene.Data);
}

#pragma endregion

#pragma region EndScene
typedef struct EndSceneData
{

	Text    Gototitle;					//타이틀로 갈 수 있는 메세지
	Text	SeeEnding[16];				//해당 엔딩을 본 경우 해당 엔딩 제목 표시
	Text    NoSeeEnding[16];			//엔딩을 못 본 경우 ???처리
	Music   TitleBGM;
	float   Volume;
	int32   FontSize;
	Image   TitleImage;
	int32	CurrentOptionNumberX;
	int32	CurrentOptionNumberY;
	int32	CurrentOptionNumber;		//메인으로 갈 시 해당 엔딩 신으로 갈 수 있게해줌 
	Image   OptionPointImage;
	int32   gotoscene[16];

} EndSceneData;
static int32 sceneNum2 = 0;
void init_end(void)
{
	CsvFile csv2;
	memset(&csv2, 0, sizeof(CsvFile));
	CreateCsvFile(&csv2, CSV_FILE_NAME);

	g_Scene.Data = malloc(sizeof(EndSceneData));
	memset(g_Scene.Data, 0, sizeof(EndSceneData));

	EndSceneData* data = (EndSceneData*)g_Scene.Data;


	data->FontSize = 25;
	for (int i = 0; i < 16; ++i)
	{
		Text_CreateText(&data->NoSeeEnding[i], TextFont, data->FontSize, L"???????????", 13);
	}
	Text_CreateText(&data->Gototitle, TextFont, 20, L"ESC - 타이틀로                                          ENTER - 회상\0", wcslen(L"BACKSPACE - 타이틀로                                          ENTER - 회상\0"));

	for (int32 j = 0; j < 16; j++)
	{
		sceneNum2 = 121 + j;
		if (IsThisEndingShown(sceneNum2))
		{
			wchar_t* SceneNameTemp2 = ParseToUnicode(csv2.Items[sceneNum2][1]);
			data->gotoscene[j] = ParseToInt(csv2.Items[sceneNum2][44]);

			Text_CreateText(&data->SeeEnding[j], TextFont, 25, SceneNameTemp2, wcslen(SceneNameTemp2));
		}
		else
		{
			data->gotoscene[j] = -1;
			Text_CreateText(&data->SeeEnding[j], TextFont, 25, L"", wcslen(L""));
		}
	}
	FreeCsvFile(&csv2);


	data->CurrentOptionNumberX = 0;
	data->CurrentOptionNumberY = 0;
	data->CurrentOptionNumber = 0;

	Image_LoadImage(&data->OptionPointImage, ARROW_IMAGE_FILE);
	data->OptionPointImage.ScaleX = 0.02f;
	data->OptionPointImage.ScaleY = 0.02f;


	Image_LoadImage(&data->TitleImage, "end.jpg");


	Audio_LoadMusic(&data->TitleBGM, "title.mp3");

	Audio_PlayFadeIn(&data->TitleBGM, INFINITY_LOOP, 2000);

	data->Volume = 1.0f;
}

void update_end(void)
{
	EndSceneData* data = (EndSceneData*)g_Scene.Data;

	if (Input_GetKeyDown(VK_UP))
	{
		if (data->CurrentOptionNumberY > 0)
		{
			data->CurrentOptionNumber--;
			data->CurrentOptionNumberY--;
		}
	}

	if (Input_GetKeyDown(VK_DOWN))
	{
		if (data->CurrentOptionNumberY < 7)
		{
			data->CurrentOptionNumber++;
			data->CurrentOptionNumberY++;
		}
	}

	if (Input_GetKeyDown(VK_LEFT))
	{
		if (data->CurrentOptionNumber > 7)
		{
			data->CurrentOptionNumberX -= 750;
			data->CurrentOptionNumber -= 8;
		}
	}

	if (Input_GetKeyDown(VK_RIGHT))
	{
		if (data->CurrentOptionNumber < 8)
		{
			data->CurrentOptionNumberX += 750;
			data->CurrentOptionNumber += 8;
		}
	}

	if (Input_GetKeyDown(VK_RETURN))
	{
		if (data->gotoscene[data->CurrentOptionNumber] != -1)
		{
			s_CurrentScene = data->gotoscene[data->CurrentOptionNumber];
			s_IsEndingScene = true;
			Scene_SetNextScene(SCENE_MAIN);
		}
	}
	if (Input_GetKeyDown(VK_ESCAPE))
	{

		Scene_SetNextScene(SCENE_TITLE);
		Audio_Stop();
	}

}

void render_end(void)
{
	EndSceneData* data = (EndSceneData*)g_Scene.Data;
	SDL_Color color1 = { 0, 0, 0, 255 };
	SDL_Color Textcolor = { 255, 255, 255, 255 };
	//배경 이미지 출력
	Renderer_DrawImage(&data->TitleImage, 0, 0);

	Renderer_DrawTextShaded(&data->Gototitle, 1020, 850, Textcolor, color1);


	//텍스트 출력

	for (int i = 0; i < 16; ++i)
	{
		if (i < 8)
		{
			if (data->gotoscene[i] != -1)
			{
				Renderer_DrawTextShaded(&data->SeeEnding[i], 100, 50 + i * 100, Textcolor, color1);

			}
			else
			{
				Renderer_DrawTextShaded(&data->NoSeeEnding[i], 100, 50 + i * 100, Textcolor, color1);
			}
		}

		if (i >= 8)
		{
			if (data->gotoscene[i] != -1)
			{
				Renderer_DrawTextShaded(&data->SeeEnding[i], 850, 50 + (i - 8) * 100, Textcolor, color1);

			}
			else
			{
				Renderer_DrawTextShaded(&data->NoSeeEnding[i], 850, 50 + (i - 8) * 100, Textcolor, color1);
			}
		}
		Renderer_DrawImage(&data->OptionPointImage, 50 + data->CurrentOptionNumberX, 50 + data->CurrentOptionNumberY * 100);
	}
}

void release_end(void)
{
	EndSceneData* data = (EndSceneData*)g_Scene.Data;

	Text_FreeText(&data->SeeEnding);
	Text_FreeText(&data->NoSeeEnding);
	Text_FreeText(&data->Gototitle);
	Image_FreeImage(&data->OptionPointImage);
	Image_FreeImage(&data->TitleImage);
	Audio_FreeMusic(&data->TitleBGM);
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

	s_IsGameClose = false;
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
	case SCENE_MAIN:
		g_Scene.Init = init_main;
		g_Scene.Update = update_main;
		g_Scene.Render = render_main;
		g_Scene.Release = release_main;
		break;
	case SCENE_END:
		g_Scene.Init = init_end;
		g_Scene.Update = update_end;
		g_Scene.Render = render_end;
		g_Scene.Release = release_end;
		break;
	}

	g_Scene.Init();

	s_nextScene = SCENE_NULL;
}

bool Scene_IsGameClose(void) {
	return s_IsGameClose;
}
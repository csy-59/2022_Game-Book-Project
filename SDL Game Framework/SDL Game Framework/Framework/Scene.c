#include "stdafx.h"
#include "Scene.h"

#include "Framework.h"
#include "Csv.h"
#include "Timer.h"

Scene g_Scene;

static ESceneType s_nextScene = SCENE_NULL;

//파일 이름 define
#define TITLE_MUSIC "title.mp3"
#define CSV_FILE_NAME "scene_data.csv"
#define ARROW_IMAGE_FILE "point.png"
#define SRINJ_IMAGE_FILE "srinj1.png"
#define TextFont "GmarketSansTTFLight.ttf"

Music   TitleBGM; //타이틀 bgm


#pragma region TitleScene
//메인에서 판별
typedef enum TitleMenu
{
	START,
	ENDING,
	EXIT
} titleMenu;

typedef struct TitleSceneData
{
	Text   LoadingText;      // 로딩 텍스트
	int32   LoadingFontSize;   // 로딩 폰트 사이즈

	Image   TitleImage;          // 타이틀 이미지

	Image   SrinjImage;         // 선택한 선택지를 표시하는 이미지
	int32   SX;               // SrinjIMage의 위치의 X좌표
	int32   SY;               // SrinjIMage의 위치의 Y좌표

	Image   StartImage;         // 크레딧 씬으로 넘어갈 수 있는 선택지의 이미지
	Image   EndingImage;      // 엔딩모음집 씬으로 넘어갈 수 있는 선택지의 이미지

	Image   BlackOutImage;     // 페이드 인 아웃 효과를 주기 위한 이미지
	int32   BlackOutAlpha;     // 페이드 인 아웃 효과를 주기 위한 이미지의 투명도

} TitleSceneData;

bool Loading = false;
titleMenu title = START;
void init_title(void)
{
	g_Scene.Data = malloc(sizeof(TitleSceneData));
	memset(g_Scene.Data, 0, sizeof(TitleSceneData));

	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;

	// 로딩
	data->LoadingFontSize = 48;  // 로딩 폰트사이즈
	Text_CreateText(&data->LoadingText, "GmarketSansTTFBold.ttf", data->LoadingFontSize, L"----- LOADING -----", 21);

	// 타이틀
	Image_LoadImage(&data->TitleImage, "title_2.jpg");

	// 선택지를 표시하는 이미지
	Image_LoadImage(&data->SrinjImage, SRINJ_IMAGE_FILE);
	data->SX = 550;
	data->SY = 750;

	// 스타트 버튼 이미지
	Image_LoadImage(&data->StartImage, "start_button.png");

	// 엔딩모음집 버튼 이미지
	Image_LoadImage(&data->EndingImage, "ending_button.png");

	// 타이틀 뮤직
	Audio_LoadMusic(&TitleBGM, TITLE_MUSIC);

	Audio_PlayFadeIn(&TitleBGM, INFINITY_LOOP, 2000);

	// 페이드인 페이드아웃 효과
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
				Scene_SetNextScene(SCENE_CREDIT); // 크레딧 씬으로 이동
			}
			else if (title == ENDING)
			{
				Loading = true;
				Scene_SetNextScene(SCENE_END);    // 엔딩모음집으로 이동
			}
		}
	}
	Image_SetAlphaValue(&data->BlackOutImage, data->BlackOutAlpha);

	if (Input_GetKeyDown(VK_ESCAPE)) // 게임 나가짐
	{
		s_IsGameClose = true;
	}
}

void render_title(void)
{
	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;

	Renderer_DrawImage(&data->TitleImage, 0, 0); // 타이틀 이미지 좌표 지정 후 랜더링

	SDL_Color color = { .a = 255 };

	static bool isShow = true;

	// 선택지 이미지와 선택지 선택 이미지 출력
	if (!Loading) // 로딩이 아닐때
	{
		Renderer_DrawImage(&data->StartImage, (WINDOW_WIDTH / 2) - (data->StartImage.Width * data->StartImage.ScaleX / 2), 750);
		Renderer_DrawImage(&data->EndingImage, (WINDOW_WIDTH / 2) - (data->EndingImage.Width * data->EndingImage.ScaleX / 2), 800);
		Renderer_DrawImage(&data->SrinjImage, data->SX, data->SY);
	}
	else // 로딩 중 일때
	{
		color.r = 255;
		color.g = 255;
		color.b = 255;
		isShow = false;
		Renderer_DrawTextSolid(&data->LoadingText, WINDOW_WIDTH / 2 - (data->LoadingText.Length * data->LoadingFontSize) / 4 + 20, 820, color);
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

#pragma region SceneData
//BGM 넘버 열거체
enum BGMType 
{
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

//각 씬의 정보를 담든 structure
typedef struct tagScene 
{
	//씬 기본 정보
	int32			SceneNumber;								//씬 넘버
	Text			SceneName;									//씬 이름

	//배경 화면
	Image			BGImage;									//배경화면
	Image			AdditionBGChangeImages[MAX_BG_CHANGE_COUNT];//배경 추가 이미지 배열
	int32			AddImageTimings[MAX_BG_CHANGE_COUNT];		//배경 추가 이미지 타이밍

	//BGM
	int32			BGMNumber;									//배경 음악 번호 위에 열거형에 정의

	//사운드 이펙트
	SoundEffect		SoundEffects[MAX_EFFECT_SOUND_COUNT];		//사운드 이펙트 배열
	int32			AddSoundEffectTimings[MAX_EFFECT_SOUND_COUNT];//사운드 이펙트 타이밍

	//아이템 이미지
	Image			ItemImage;									//아이템 이미지
	int32			AddItemImageTiming;							//아이템 이미지 등장 타이밍
	int32			FadeItemImageTiming;						//아이템 이미지 사라지는 타이밍

	//두근 효과
	int32			AddPoundingItemImageTiming;					//아이템 이미지 두근 거리는 효과 넣는 타이밍
	int32			FadePoundingItemImageTiming;				//아이템 이미지 두근 거리는 효과 그만하는 타이밍

	//밀기 효과
	int32			ImagePushingType;							//이미지(화면, 아이템) 밀기 유형(-1: 없음, 0: 배경만, 1: 아이템만)
	int32			ImagePushingTiming;							//이미지 밀기 시작 타이밍
	int32			ImagePushingX;								//이미지 민 후 변화한 x값
	int32			ImagePushingY;								//이미지 민 후 변화한 y값

	//흔들림 효과
	int32			ShakingTimging;								//화면 흔들기 타이밍
	int32			ShakingX;									//화면 흔들기에서 사용할 X값
	int32			ShakingY;									//화면 흔들기에서 사용할 X값
	
	//텍스트 
	int32			DialogCount;									//텍스트 갯수
	Text			DialogList[MAX_TEXT_SET_COUNT][MAX_TEXT_COUNT];	//텍스트 배열

	//선택지
	int32			OptionCount;								//옵션 갯수
	Text			OptionList[MAX_OPTION_COUNT];				//옵션 텍스트 배열

	//다음씬
	int32			NextSceneNumberList[MAX_OPTION_COUNT];		//옵션 선택시 넘어가는 씬 넘버

	//다음씬에서 bgm 변경 여부
	bool			IsNextSceneBGMChange[MAX_OPTION_COUNT];			//다음 씬에서 BGMNUM이 바뀌는 확인
} SceneStruct;

void GetSceneData(int32 sceneNum, SceneStruct* scene) 
{
	//csv 파일 열기
	CsvFile csv;
	memset(&csv, 0, sizeof(CsvFile));
 	CreateCsvFile(&csv, CSV_FILE_NAME);

	//이상한 씬넘버가 들어오면 나가기
	if (csv.Items[sceneNum] == NULL) 
	{
		printf("ERROR!!! WORNG SCENE NUMBER");
		return;
	}

	int32 columCount = 0;

	//씬 번호
 	scene->SceneNumber = ParseToInt(csv.Items[sceneNum][columCount++]);

	
	//씬 이름
	wchar_t* SceneNameTemp = ParseToUnicode(csv.Items[sceneNum][columCount++]);
	Text_CreateText(&scene->SceneName, TextFont, 25, SceneNameTemp, wcslen(SceneNameTemp));


	//배경 이미지
	Image_LoadImage(&scene->BGImage, ParseToAscii(csv.Items[sceneNum][columCount++]));


	//bgm
	scene->BGMNumber = ParseToInt(csv.Items[sceneNum][columCount++]);

		
	//배경 전환 이미지
	for (int32 j = 0; j < MAX_BG_CHANGE_COUNT;j++) 
	{
		int32 additionalImagePoint = ParseToInt(csv.Items[sceneNum][columCount + 1]);
		scene->AddImageTimings[j] = additionalImagePoint;
		
		if (additionalImagePoint > -1) 
		{
			Image_LoadImage(&scene->AdditionBGChangeImages[j], ParseToAscii(csv.Items[sceneNum][columCount]));
		}
		columCount += 2;
	}


	//이펙트 사운드
	for (int32 j = 0; j < MAX_EFFECT_SOUND_COUNT; j++) 
	{
		int32 addEffectSoundPoint = ParseToInt(csv.Items[sceneNum][columCount + 1]);
 		scene->AddSoundEffectTimings[j] = addEffectSoundPoint;
		
		if (addEffectSoundPoint > -1) 
		{
			Audio_LoadSoundEffect(&scene->SoundEffects[j], ParseToAscii(csv.Items[sceneNum][columCount]));
		}
		columCount += 2;
	}


	//아이템 이미지
	int32 itemImagePoint = ParseToInt(csv.Items[sceneNum][columCount + 1]);
	scene->AddItemImageTiming = itemImagePoint;
	if (itemImagePoint > -1) 
	{
		Image_LoadImage(&scene->ItemImage, ParseToAscii(csv.Items[sceneNum][columCount]));
		scene->FadeItemImageTiming = ParseToInt(csv.Items[sceneNum][columCount + 2]);
	}
	columCount += 3;


	//아이템 두근두근 효과
	int32 poundingPoint = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->AddPoundingItemImageTiming = poundingPoint;
	if (poundingPoint > -1) 
	{
		scene->FadePoundingItemImageTiming = ParseToInt(csv.Items[sceneNum][columCount]);
	}
	columCount++;


	//화면 흔들기 효과
	scene->ShakingTimging = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->ShakingX = 0;
	scene->ShakingY = 0;


	//화면 밀기 효과
	scene->ImagePushingType = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->ImagePushingTiming = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->ImagePushingX = 0;
	scene->ImagePushingY = 0;


	//텍스트 데이터 저장
	int32 dialogCount = ParseToInt(csv.Items[sceneNum][columCount++]);
	scene->DialogCount = dialogCount;
	for (int32 j = 0; j < MAX_TEXT_SET_COUNT;j++) 
	{
		if (dialogCount > j) 
		{
			wchar_t* originalData = ParseToUnicode(csv.Items[sceneNum][columCount]);
			wchar_t lineData[2048];
			int32 originalIndex = 0, lineDataIndex = 0, dialogListIndex = 0;
			wchar_t preChar = '\0';

			//텍스트 줄바꿈 단위로 나눠 저장
			while (true) 
			{
				//그냥 입력일 경우
				if (*(originalData + originalIndex) != '\n' && *(originalData + originalIndex) != '\0') {
					//"가 아닐 경우 그냥 넣어준다.
					if (*(originalData + originalIndex) != '\"') 
					{
						lineData[lineDataIndex++] = *(originalData + originalIndex);
						preChar = *(originalData + originalIndex);
					}
					// "가 연속으로 있을 경우 넣어준다.
					else 
					{
						if (preChar == *(originalData + originalIndex)) 
						{
							lineData[lineDataIndex++] = *(originalData + originalIndex);
							preChar = '\0';
						}
						else 
						{
							preChar = *(originalData + originalIndex);
						}
					}
				}
				//개행이거나 널일 경우 text를 넣어준다.
				else 
				{
					lineData[lineDataIndex++] = '\0';
					Text_CreateText(&scene->DialogList[j][dialogListIndex], "GmarketSansTTFLight.ttf", 25, lineData, wcslen(lineData));
					dialogListIndex++;
					lineDataIndex = 0;

					if (*(originalData + originalIndex) == '\0') 
					{
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
	if (scene->OptionCount > 0) 
	{
		for (int32 j = 0; j < MAX_OPTION_COUNT;j++) 
		{
			if (scene->OptionCount > j) 
			{
				wchar_t* temp = ParseToUnicode(csv.Items[sceneNum][columCount]);
				Text_CreateText(&scene->OptionList[j], "GmarketSansTTFLight.ttf", 20, temp, wcslen(temp));

				columCount++;
				scene->NextSceneNumberList[j] = ParseToInt(csv.Items[sceneNum][columCount]);
				//다음씬에서 bgm이 바뀌는지 확인하는 것
				if (scene->NextSceneNumberList[j] > -1) 
				{
					scene->IsNextSceneBGMChange[j] =
						ParseToInt(csv.Items[scene->NextSceneNumberList[j]][3]) != scene->BGMNumber;
				}
				columCount++;
			}
		}
	}
	else 
	{
		scene->NextSceneNumberList[0] = ParseToInt(csv.Items[sceneNum][++columCount]);
		if (scene->NextSceneNumberList[0] > -1) 
		{
			scene->IsNextSceneBGMChange[0] =
				ParseToInt(csv.Items[scene->NextSceneNumberList[0]][3]) != scene->BGMNumber;
		}
	}


	FreeCsvFile(&csv);
}

void Scene_Clear(SceneStruct* scene) 
{
	//이미지 해제
	Image_FreeImage(&scene->BGImage);
	for (int32 j = 0; j < MAX_BG_CHANGE_COUNT;j++) 
	{
		if (scene->AddImageTimings[j] > -1) 
		{
			Image_FreeImage(&scene->AdditionBGChangeImages[j]);
		}
		else 
		{
			break;
		}
	}
	Image_FreeImage(&scene->ItemImage);

	//오디오 해제
	for (int32 j = 0; j < MAX_EFFECT_SOUND_COUNT; j++) 
	{
		if (scene->AddSoundEffectTimings[j] > -1) 
		{
			Audio_FreeSoundEffect(&scene->SoundEffects[j]);
		}
		else 
		{
			break;
		}
	}

	//텍스트 해제
	if (scene->SceneName.Length > 0) 
	{
		Text_FreeText(&scene->SceneName);
	}
	for (int32 j = 0; j < scene->DialogCount;j++) 
	{
		int32 k = 0;
		while (scene->DialogList[j][k].Length > 0) 
		{
			Text_FreeText(&scene->DialogList[j][k]);
			k++;
		}
		Text_FreeText(&scene->DialogList[j][k]);
	}
	for (int32 j = 0; j < scene->OptionCount;j++) 
	{
		Text_FreeText(&scene->OptionList[j]);
	}
}

#pragma endregion

#pragma region CreditScene
typedef struct CreditSceneData // 크레딧 구조체
{

	// 이미지
	Image    CreditImage[3]; // 이미지 삽입
	int32    X;              // 이미지 x 좌표
	int32    Y;              // 이미지 y 좌표
} CreditSceneData;

int32 s_CurrentPageCredit;

void init_credit(void)
{
	// 기본 데이터 가져오기
	g_Scene.Data = malloc(sizeof(CreditSceneData));
	memset(g_Scene.Data, 0, sizeof(CreditSceneData));

	// 신 데이터 받기
	CreditSceneData* data = (CreditSceneData *)g_Scene.Data;



	// 이미지 출력, 좌표
	Image_LoadImage(&data->CreditImage[0], "credit.jpg");
	Image_LoadImage(&data->CreditImage[1], "1.jpg");
	Image_LoadImage(&data->CreditImage[2], "1-4.jpg");
	data->X = 0;
	data->Y = 0;

	s_CurrentPageCredit = 0;
}

void update_credit(void)
{
	CreditSceneData* data = (CreditSceneData *)g_Scene.Data;

	// 스페이스 입력시 다음신 
	if (Input_GetKeyDown(VK_SPACE))
	{
		if (s_CurrentPageCredit < 2)
		{
			s_CurrentPageCredit++;
		}
		else
		{
			Scene_SetNextScene(SCENE_MAIN);
			Audio_Stop();
		}
	}

	// esc 입력시 게임 종료
	if (Input_GetKeyDown(VK_ESCAPE))
	{
		s_IsGameClose = true;
	}
}

void render_credit(void)
{
	// 크레딧 이미지 출력
	CreditSceneData* data = (CreditSceneData *)g_Scene.Data;
	Renderer_DrawImage(&data->CreditImage[s_CurrentPageCredit], data->X, data->Y);
}

void release_credit(void)
{
	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;

	// 브금 
	Audio_FreeMusic(&TitleBGM);

	// 이미지
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
static int32 s_CurrentScene = 1;	//현재 씬 넘버

#pragma region MainScene
typedef struct tagMainScene 
{
	SceneStruct Scene;								//씬 정보	

	//씬 전환
	Image		BlackOutImage;						//씬 이동시 페이드 인/아웃에 적용할 검은 배경
	int32		BlackOutAlpha;						//검은 배경의 투명도
	bool		isSceneChanging;					//현재 씬이 전환되고 있는 중인지 판별

	//배경
	Image*		CurrentBGImage;						//현재 보여주고 있는 배경 이미지. 다른것이 아닌 이것만 랜더 해준다.
	int32		CurrentBGChangeNumber;				//현재 바꿔진 배경 화면 번수
	int32		BGAlpha;							//배경화면의 투명도. 배경 전환시 사용
	bool		isBGChanged;						//배경화면이 전환중인지
	//기타 화면
	Image		UiImage;							//UI 화면
	Image		PopupImage;							//팝업 화면
	bool		isShowingPopUp;						//팝업 화면을 보여주고 있는지

	//텍스트
	Text*		ShowText;							//표현해야하는 텍스트 배열
	Text		NullText;							//텍스트에 사용할 빈 텍스트
	SDL_Color	TextColor;							//텍스트의 색상. 여기서 투명도 조절로 텍스트 페이드 인/아웃 처리
	int32		CurrentTextNumber;					//현재 보여주고 있는 텍스트 번호

	//선택지
	Image		OptionPointImage;					//선택지를 선택중인걸 확인 시켜줄 주사기 포인터
	SDL_Color	OptionColors[MAX_OPTION_COUNT];		//선택지 마다의 색 표현. 이걸 활용하여 투명도를 조절한다.
	int32		CurrentOptionNumber;				//현재 선택중인 선택지 번호
	bool		showOptions;						//선택지를 보여줘야 하는지
	
	//아이템
	bool		showItemImage;						//아이템 이미지를 보여줘야 할지 판별

	//두근거리는 효과
	float		ElapsedTime;						//두근두근 효과에 사용할 시간
	bool		isItemPounding;						//두근 거리는 효과를 보여줘야할지
	bool		isItemBigger;						//두근 거리는 효과에서 커져야 할지
	
	//화면 흔들림
	float		ElapsedShakingTime;					//화면 흔들림 효과에 적용할 시간
	bool		isShaking;							//화면 흔들림 효과가 적용되고 있는지
	bool		isEnter;							//화면 흔들리면 못 넘어가게 하기

	//이미지 밀기
	bool		isImagePushing;						//화면을 밀고 있는지 판별

	//효과음
	int32		CurrentSoundEffectNumber;			//현재 표현중인 효과음 번수
	bool		isPlayedEffect;						//효과음을 실행했는지

} MainScene;

static int32 SCROLE_SPEED = 20;						//화면 밀기 속도

static Music CurrentBGM;							//현재 플래이 중인 bgm
static int32 CurrentBGMNumber = BGM_TITLE;			//현재 플래이 중인 bgm 번호

//bgm 변경
void ChangeBGM(int32 BGMNum) 
{
	//기존 음악 free
	Audio_FreeMusic(&CurrentBGM);

	//새로운 넘버로 현재 플래이 중인 음악 번호 수정
	CurrentBGMNumber = BGMNum;

	//번호에 따라 다른 음악 출력(상단 BGM_TYPE 열거채 참고)
	switch (BGMNum) 
	{
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

//엔딩 모음집 관련 파일 입출력
//해당 엔딩을 보았는지
int32 IsThisEndingShown(int32 SceneNum) 
{
	FILE* fp = NULL;

	//우선 파일을 읽어서 데이터를 받아 어디에 저장할지 판별한다.
	errno_t err = fopen_s(&fp, "Asset/Data/endingLog.txt", "r");
	
	//파일을 열수 없으면 에러 출력
	if (err != 0) 
	{
		printf("ERROR!!! CAN'T READ endingLog.txt\n");
		return;
	}

	//파일 값 저장
	char endNum[256] = "";
	char* endNumPointer = NULL;
	char* spaceSearchPointer = NULL;
	bool isEndingShown = false;

	while (fgets(endNum, sizeof(endNum), fp) != NULL) 
	{
		int32 tempEndNum = 0;//임시로 저장해줄 곳
		if (strlen(endNum) != 0) 
		{
			spaceSearchPointer = strtok_s(endNum, ",", &endNumPointer);
			while (spaceSearchPointer != NULL) 
			{
				if (tempEndNum == 0) 
				{
					tempEndNum = atoi(spaceSearchPointer);
					if (tempEndNum == SceneNum) 
					{
						isEndingShown = true;
					}
				}
				else if (tempEndNum > SceneNum) 
				{
					fclose(fp);
					return -1;
				}
				else if(isEndingShown) 
				{
					int32 tempPrevNum = atoi(spaceSearchPointer);
					fclose(fp);
					return tempPrevNum;
				}
				spaceSearchPointer = strtok_s(NULL, ",", &endNumPointer);
			}
			endNumPointer = NULL;
		}
		else 
		{
			break;
		}
	}
	fclose(fp);

	return -1;
}
//엔딩 저장
void RecordThisEnding(int32 SceneNum) 
{
	int32 EndingNum = 0;

	//엔딩씬 바로 전 씬만 저장
 	switch (SceneNum) 
	{
	case 7:
	case 9:
	case 49:
	case 55:
	case 82:
	case 104:
	case 113:
		EndingNum = 121;
		break;

	case 14:
		EndingNum = 122;
		break;

	case 17:
	case 44:
		EndingNum = 123;
		break;

	case 19:
	case 34:
	case 38:
	case 43:
		EndingNum = 124;
		break;

	case 27:
		EndingNum = 125;
		break;

	case 46:
		EndingNum = 126;
		break;

	case 99:
		EndingNum = 127;
		break;

	case 51:
		EndingNum = 128;
		break;

	case 57:
		EndingNum = 129;
		break;

	case 58:
	case 63:
	case 80:
	case 105:
		EndingNum = 130;
		break;

	case 61:
	case 70:
	case 108:
		EndingNum = 131;
		break;

	case 73:
	case 96:
	case 115:
		EndingNum = 132;
		break;

	case 86:
		EndingNum = 133;
		break;

	case 87:
		EndingNum = 134;
		break;

	case 118:
		EndingNum = 135;
		break;

	case 102:
		EndingNum = 136;
		break;
		
	case 120:
		EndingNum = 137;
		break;

	default: 
		return;
		break;
	}

	FILE* fp = NULL;
	fopen_s(&fp, "Asset/Data/endingLog.txt", "r");

	//파일이 열리지 않으면 에러 출력
	if (fp == NULL) 
	{
		printf("ERROR!!! CAN'T READ endingLog.txt\n");
		fclose(fp);
		return;
	}

	//엔딩 리스트를 불러와 해당 엔딩을 보았는지 확인하고 저장한다.
	bool endingList[17] = { false }; // 엔딩 봤는지 여부
	int32 endingPrevList[17] = { 0 }; // 엔딩 전 씬이 어딘지

	//지금 들어온 값 저장
	endingList[EndingNum - 121] = true;
	endingPrevList[EndingNum - 121] = SceneNum;

	//파일 값 저장
	char endNum[256] = "";
	char* endNumPointer = NULL;
	char* spaceSearchPointer = NULL;
	char changeNum[256] = ""; //atoi로 변환 할 문자열 저장
	int32 changeNumCount = 0;

	while (fgets(endNum, sizeof(endNum), fp) != NULL) 
	{
		int32 tempEndNum = 0;//임시로 저장해줄 곳
		if (strlen(endNum) != 0) 
		{
			spaceSearchPointer = strtok_s(endNum, ",", &endNumPointer);
			while (spaceSearchPointer != NULL) 
			{
				if (tempEndNum == 0) 
				{
					tempEndNum = atoi(spaceSearchPointer);
					endingList[tempEndNum - 121] = true;
				}
				else 
				{
					int32 prevSceneNum = atoi(spaceSearchPointer);
					if (tempEndNum == EndingNum) 
					{
						if (prevSceneNum > SceneNum) 
						{
							endingPrevList[tempEndNum - 121] = prevSceneNum;
						}
						else 
						{
							endingPrevList[tempEndNum - 121] = SceneNum;
						}
					}
					else 
					{
						endingPrevList[tempEndNum - 121] = prevSceneNum;
					}
				}
				spaceSearchPointer = strtok_s(NULL, ",", &endNumPointer);
			}
			endNumPointer = NULL;
		}
		else 
		{
			break;
		}
	}
	fclose(fp);

	//해당 데이터를 기반으로 데이터 다시 저장
	fopen_s(&fp, "Asset/Data/endingLog.txt", "w");
	if (fp == NULL) 
	{
		printf("ERROR!!! CAN'T READ endingLog.txt\n");
		return;
	}

	//데이터에 넣을 값 지정
	endNum[0] = '\0';
	for (int32 i = 0; i < 17; i++) 
	{
		if (endingList[i]) 
		{
			char tempNum[10];
			_itoa_s(i + 121, tempNum, sizeof(tempNum), 10);
			strcat_s(endNum, sizeof(endNum), tempNum);
			strcat_s(endNum, sizeof(endNum), ",");
			_itoa_s(endingPrevList[i], tempNum, sizeof(tempNum), 10);
			strcat_s(endNum, sizeof(endNum), tempNum);
			strcat_s(endNum, sizeof(endNum), "\n");
		}
	}
	fputs(endNum, fp);

	fclose(fp);
}

void init_main(void)
{
	//기본 데이터 가져오기
	g_Scene.Data = malloc(sizeof(MainScene));
	memset(g_Scene.Data, 0, sizeof(MainScene));

	MainScene* data = (MainScene*)g_Scene.Data;

	//씬 정보 받기
	GetSceneData(s_CurrentScene, &data->Scene);


	//####씬 전환
	data->isSceneChanging = false; //씬 전환중인지


	//####배경
	data->CurrentBGImage = &data->Scene.BGImage; //현재 보여주는 bg 이미지
	data->BGAlpha = 255;
	data->CurrentBGChangeNumber = 0; //현재 배경 번호
	data->isBGChanged = false; //배경 변하고 있는지
	//기타 화면
	Image_LoadImage(&data->UiImage, "UI.jpg"); //UI 이미지
	Image_LoadImage(&data->PopupImage, "PopUp.png"); //팝업 이미지
	data->isShowingPopUp = false; //팝업 화면 띄우고 있는지


	//####BGM 관련
	if (CurrentBGMNumber != data->Scene.BGMNumber) 
	{
		ChangeBGM(data->Scene.BGMNumber);
		Audio_PlayFadeIn(&CurrentBGM, INFINITY_LOOP, 2000);
	}
	else if (!Audio_IsMusicPlaying() || Audio_IsMusicFading()) 
	{
		ChangeBGM(data->Scene.BGMNumber);
		Audio_PlayFadeIn(&CurrentBGM, INFINITY_LOOP, 2000);
	}


	//####텍스트
	data->CurrentTextNumber = 0; //현재 텍스트 번째 수 초기화
	Text_CreateText(&data->NullText, TextFont, 25, L"", 0); //필요한 NULLText 세팅

	data->TextColor.a = 255; //텍스트 색상 조절
	data->TextColor.r = 255;
	data->TextColor.g = 255;
	data->TextColor.b = 255;

	data->ShowText = NULL; //표현할 텍스트 초기화


	//####선택지
	data->CurrentOptionNumber = 0; //현재 옵션 번호 초기화
	data->showOptions = false; //선택지 보여주는지
	
	Image_LoadImage(&data->OptionPointImage, SRINJ_IMAGE_FILE); //선택지 표시 이미지 세팅
	data->OptionPointImage.ScaleX = 0.35f;
	data->OptionPointImage.ScaleY = 0.35f;
	
	for (int32 i = 0; i < data->Scene.OptionCount;i++) 
	{ //선택지 텍스트 요소 값
		data->OptionColors[i].a = 125;
		data->OptionColors[i].r = 225;
		data->OptionColors[i].g = 225;
		data->OptionColors[i].b = 225;
	}


	//####아이템
	data->showItemImage = false; //아이템 이미지를 보여주고 있는지


	//####두근거리는 효과
	data->ElapsedTime = 0.0f; //두근 거리는 효과에서 사용하는 시간


	//####화면 흔들림
	data->ElapsedShakingTime = 0.0f; //화면 흔들리는 시간 초기화
	data->isShaking = false; //흔들림 효과 중인지
	data->isEnter = false; //흔들림 중에 넘어가기 금지


	//####이미지 밀기
	data->isImagePushing = false; //이미지 미는 중인지
	data->isItemPounding = false; //두근거리는 효과 적용중인지
	data->isItemBigger = false; //두근거리는 효과에서 크기가 커져야 하는지


	//####효과음
	data->CurrentSoundEffectNumber = 0; //효과음 번호
	data->isPlayedEffect = false;//효과음 판별 초기화


	//##암전 효과
	data->BlackOutAlpha = 255;
	Image_LoadImage(&data->BlackOutImage, "black.jpg");
	Image_SetAlphaValue(&data->BlackOutImage, data->BlackOutAlpha);


	//엔딩 씬이라면 파일에 저장
	RecordThisEnding(data->Scene.SceneNumber);


	//현재 씬이 크래딧이면 화면 밀기 속도 조절
	if (s_CurrentScene == 138) 
	{
		SCROLE_SPEED = 3;
	}
}

void update_main(void)
{
  	MainScene* data = (MainScene*)g_Scene.Data;

	//보통 씬일 경우
	if (!data->isSceneChanging) 
	{
		// 배경 변경 중일 경우
		if (data->isBGChanged) 
		{
			if (data->BGAlpha > 0) 
			{
				if (data->BGAlpha - 25 < 0) 
				{
					data->BGAlpha = 0;
				}
				else 
				{
					data->BGAlpha -= 25;
				}
			}
			else 
			{
				data->isBGChanged = false;
				data->CurrentBGImage = &data->Scene.AdditionBGChangeImages[data->CurrentBGChangeNumber];
				data->ShowText = &data->Scene.DialogList[data->CurrentTextNumber];
				data->TextColor.a = 0;
			}
		}
		// 배경 변경 중일 때(배경의 알파값이 달라짐)
		else if (data->BGAlpha < 255) 
		{
			if (data->BGAlpha + 25 > 255) 
			{
				data->BGAlpha = 255;
			}
			else 
			{
				data->BGAlpha += 25;
			}
		}
		// 씬 최초 불러오기
		else if (data->BlackOutAlpha > 0) 
		{
			if (!data->isShowingPopUp) 
			{
				data->BlackOutAlpha -= 5;
			}
		}
		//이외 입력 정상 받음
		else {
			//키보드 값 입력
			if (Input_GetKeyDown(VK_SPACE)) 
			{
				//출력 텍스트 조절
				if (data->CurrentTextNumber < data->Scene.DialogCount) 
				{
					data->ShowText = &data->Scene.DialogList[data->CurrentTextNumber];
					data->CurrentTextNumber++;
					Audio_FadeOutSoundEffect(800);
					data->Scene.ShakingX = 0;
					data->Scene.ShakingY = 0;

					//배경 이미지 변경
					for (int32 i = data->CurrentBGChangeNumber; i < MAX_BG_CHANGE_COUNT;i++) 
					{
						if (data->Scene.AddImageTimings[i] > -1) 
						{
							if (data->CurrentTextNumber + 1 == data->Scene.AddImageTimings[i]) 
							{
								data->isBGChanged = true;
								data->ShowText = &data->NullText;
								data->CurrentBGChangeNumber = i;
								data->Scene.ImagePushingX = 0;
								data->Scene.ImagePushingY = 0;
								data->isImagePushing = false;
								break;
							}
						}
						else 
						{
							break;
						}
					}

					data->isPlayedEffect = false;

					data->TextColor.a = 0;
				}
				if (data->CurrentTextNumber >= data->Scene.DialogCount && data->Scene.OptionCount <= 0) 
				{
					data->isSceneChanging = true;
 					if (data->Scene.IsNextSceneBGMChange[0]) 
					{
						Audio_FadeOut(1800);
						Audio_FadeOutSoundEffect(1800);
					}
					s_CurrentScene = data->Scene.NextSceneNumberList[data->CurrentOptionNumber];
				}
			}
		}

		//텍스트 표시가 필요한 경우
		if (data->CurrentTextNumber < data->Scene.DialogCount) 
		{
			int32 i = 0;
			data->ShowText = data->Scene.DialogList[data->CurrentTextNumber];
			if (data->TextColor.a < 255) 
			{
				data->TextColor.a += 5;
			}
		}
		//옵션이 나와야하는 경우
		else 
		{
			if (data->Scene.OptionCount > 0) 
			{
				data->showOptions = true;
			}

			//선택지 선택
			int32 optionCount = data->Scene.OptionCount;

			data->OptionColors[data->CurrentOptionNumber].a = 125;
			if (Input_GetKeyDown('1') || Input_GetKeyDown(VK_NUMPAD1)) 
			{
				if (optionCount >= 1) 
				{
					data->CurrentOptionNumber = 0;
				}
			}
			if (Input_GetKeyDown('2') || Input_GetKeyDown(VK_NUMPAD2)) 
			{
				if (optionCount >= 2)
				{
					data->CurrentOptionNumber = 1;
				}
			}
			if (Input_GetKeyDown('3') || Input_GetKeyDown(VK_NUMPAD3)) 
			{
				if (optionCount >= 3) 
				{
					data->CurrentOptionNumber = 2;
				}
			}

			//위 아래 키로 선택지 선택
			if (Input_GetKeyDown(VK_UP)) 
			{
				if (data->CurrentOptionNumber > 0) 
				{
					data->CurrentOptionNumber--;
				}
			}

			if (Input_GetKeyDown(VK_DOWN)) 
			{
				if (data->CurrentOptionNumber < data->Scene.OptionCount - 1) 
				{
					data->CurrentOptionNumber++;
				}
			}

			data->OptionColors[data->CurrentOptionNumber].a = 255;

			//선택지 선택
			if (Input_GetKeyDown(VK_RETURN) && !data->isShowingPopUp) 
			{
				data->isSceneChanging = true;
				s_CurrentScene = data->Scene.NextSceneNumberList[data->CurrentOptionNumber];
				if (data->Scene.IsNextSceneBGMChange[data->CurrentOptionNumber]) 
				{
					Audio_FadeOut(1800);
					Audio_FadeOutSoundEffect(1800);
				}
			}
		}

	}
	//씬 전환 처리(페이드 인/아웃)
	else 
	{
		if (data->BlackOutAlpha < 255) 
		{
			data->BlackOutAlpha += 5;
		}
		else 
		{
			data->isSceneChanging = false;
			//다음 씬이 -1, 즉 엔딩일때는 타이틀로 돌아감. 아니면 다음 씬을 구성
			if (s_IsEndingScene && !data->isShowingPopUp) 
			{
				if (s_CurrentScene != -1) 
				{
					for (int32 i = 0; i < data->Scene.OptionCount + 1; i++) 
					{
						if (data->Scene.NextSceneNumberList[i] >= 121) 
						{
							s_CurrentScene = data->Scene.NextSceneNumberList[i];
							Scene_SetNextScene(SCENE_MAIN);
						}
					}
				}
				else 
				{
					s_IsEndingScene = false;
					Scene_SetNextScene(SCENE_END);
					s_CurrentScene = 1;
				}
			}
			else if (s_CurrentScene != -1 && !data->isShowingPopUp) 
			{
				Scene_SetNextScene(SCENE_MAIN);
			}
			else 
			{
				s_CurrentScene = 1;
				Scene_SetNextScene(SCENE_TITLE);
			}
			data->isShowingPopUp = false;
		}
	}

	//타이틀로 가는 esc 팝업 화면
	if (Input_GetKeyDown(VK_ESCAPE)) 
	{
		if (!data->isShowingPopUp) 
		{
			data->isShowingPopUp = true;
			Audio_Pause();
			Audio_StopSoundEffect();
			data->BlackOutAlpha = 125;
		}
		else 
		{
			data->isShowingPopUp = false;
			Audio_Resume();
			Audio_ResumeSoundEffect();
		}
	}

	//팝업
	if (data->isShowingPopUp) 
	{
		if (Input_GetKeyDown(VK_RETURN)) 
		{
			data->isSceneChanging = true;
			s_CurrentScene = -1;
		}
	}

	//팝업 표현 중일 때는 효과 적용X
	if (!data->isShowingPopUp) 
	{
		if (!data->isPlayedEffect) 
		{
			//이펙트 사운드 추가
			for (int32 i = data->CurrentSoundEffectNumber; i < MAX_EFFECT_SOUND_COUNT; i++) 
			{
				if (data->Scene.AddSoundEffectTimings[i] > -1)
				{
					if (data->CurrentTextNumber + 1 == data->Scene.AddSoundEffectTimings[i]) 
					{
						Audio_PlaySoundEffect(&data->Scene.SoundEffects[i], 0);
						data->isPlayedEffect = true;
						data->CurrentBGChangeNumber = i;
						break;
					}
				}
				else 
				{
					break;
				}
			}
		}

		//아이템 이미지 적용
		if (data->CurrentTextNumber + 1 >= data->Scene.AddItemImageTiming && data->CurrentTextNumber + 1 < data->Scene.FadeItemImageTiming) 
		{
			data->showItemImage = true;
		}
		else
		{
			data->showItemImage = false;
		}

		//배경 밀어내는 효과 적용
		if (data->CurrentTextNumber + 1 == data->Scene.ImagePushingTiming) 
		{

			//배경 이동
			if (data->Scene.ImagePushingType == 0) 
			{
				//아직 끝으로 가지 않았을 경우
				if (data->Scene.ImagePushingY - SCROLE_SPEED > (data->CurrentBGImage->Height - WINDOW_HEIGHT) * -1) 
				{
					data->Scene.ImagePushingY -= SCROLE_SPEED;
				}
				//끝에 다다랐을 때
				else 
				{
					if (s_CurrentScene == 138) {
						data->isSceneChanging = true;
						s_CurrentScene = -1;
					}
					data->isImagePushing = false;
				}
			}
			//아이템 이동
			else if (data->Scene.ImagePushingType == 1) 
			{
				//아직 끝으로 가지 않았을 경우 
				if (data->Scene.ImagePushingX > (data->Scene.ItemImage.Width) * -1) 
				{
					data->Scene.ImagePushingX -= SCROLE_SPEED;
				}
				//끝에 다다랐을 때
				else 
				{
					data->isImagePushing = false;
				}
			}
			//이상한 값
			else 
			{
				printf("ERROR!! WORNG PUSHING TYPE\n");
				data->isImagePushing = false;
			}

		}

		//아이템 이미지에 효과 적용(두근두근)
		if (data->CurrentTextNumber + 1 >= data->Scene.AddPoundingItemImageTiming &&
			data->CurrentTextNumber + 1 < data->Scene.FadePoundingItemImageTiming) 
		{
			data->isItemPounding = true;
			if (!data->isItemBigger) 
			{
				if (data->Scene.ItemImage.ScaleX < 1.5f) 
				{
					data->Scene.ItemImage.ScaleX += 0.15f;
					data->Scene.ItemImage.ScaleY += 0.15f;
				}
				else 
				{
					data->isItemBigger = true;
				}
			}
			else 
			{
				if (data->Scene.ItemImage.ScaleX > 1.0f) 
				{
					data->Scene.ItemImage.ScaleX -= 0.15f;
					data->Scene.ItemImage.ScaleY -= 0.15f;
				}
				else if (data->Scene.ItemImage.ScaleX <= 1.0f) 
				{
					if (data->ElapsedTime < 0.5f) 
					{
						data->ElapsedTime += Timer_GetDeltaTime();
					}
					else 
					{
						data->ElapsedTime = 0.0f;
						data->isItemBigger = false;
					}
				}
			}
		}
		else 
		{
			data->isItemPounding = false;
			data->Scene.ItemImage.ScaleX = 1.0f;
			data->Scene.ItemImage.ScaleY = 1.0f;
		}


		//화면 흔들리는 효과
		if (data->Scene.ShakingTimging == data->CurrentTextNumber + 1)
		{
			if (!data->isShaking)
			{
				data->ElapsedShakingTime += Timer_GetDeltaTime();

				float shakingTime = 0.5f;
				if (!data->BlackOutAlpha == 0) 
				{
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
	if (data->Scene.ImagePushingType != 0) 
	{
		Renderer_DrawImage(data->CurrentBGImage, data->Scene.ShakingX, data->Scene.ShakingY);
	}
	else 
	{
		Renderer_DrawImage(data->CurrentBGImage, 
			data->Scene.ImagePushingX + data->Scene.ShakingX,
			data->Scene.ImagePushingY + data->Scene.ShakingY);
	}

	//아이템 이미지 출력
	if (data->showItemImage) 
	{
		//두근 거리는 효과
		if (data->isItemPounding) 
		{
			Renderer_DrawImage(&data->Scene.ItemImage,
				(data->Scene.ItemImage.Width * (1.0f - data->Scene.ItemImage.ScaleX)) / 2 + data->Scene.ShakingX,
				(data->Scene.ItemImage.Height * (1.0f - data->Scene.ItemImage.ScaleY)) / 2 + data->Scene.ShakingY);
		}
		else if (data->Scene.ImagePushingType != 1) 
		{
			Renderer_DrawImage(&data->Scene.ItemImage, data->Scene.ShakingX, data->Scene.ShakingY);
		}
		//이미지 밀어 올리기
		else 
		{
			Renderer_DrawImage(&data->Scene.ItemImage,
				data->Scene.ImagePushingX, data->Scene.ImagePushingY);
		}
	}

	//UI 출력
	if (s_CurrentScene < 121) 
	{
		Renderer_DrawImage(&data->UiImage, data->Scene.ShakingX, data->Scene.ShakingY);
	}

	int32 finalTextPosY = 0;
	SDL_Color color1 = { 14, 14, 14, 255 };
	//텍스트 출력
	if (data->ShowText != NULL && !data->isBGChanged) 
	{
		int32 i = 0;
		while (data->ShowText[i].Length != 0) 
		{
			Renderer_DrawTextShaded(&data->ShowText[i], 250 + data->Scene.ShakingX, 645 + i * 40 + data->Scene.ShakingY, data->TextColor, color1);
			finalTextPosY = 645 + i * 40 + data->Scene.ShakingY;
			i++;
		}
	}

	//선택지 출력
	if (finalTextPosY == 0) 
	{
		finalTextPosY = 645;
	}
	if (data->showOptions) 
	{
		for (int32 i = 0; i < data->Scene.OptionCount;i++) 
		{
			Renderer_DrawTextShaded(&data->Scene.OptionList[i], 250 + data->Scene.ShakingX, finalTextPosY + 65 + i * 40 + data->Scene.ShakingY, data->OptionColors[i], color1);
			Renderer_DrawTextSolid(&data->Scene.OptionList[i], 250 + data->Scene.ShakingX, finalTextPosY + 65 + i * 40 + data->Scene.ShakingY, data->OptionColors[i]);
		}
		Renderer_DrawImage(&data->OptionPointImage, 200 + data->Scene.ShakingX, finalTextPosY + 68 + data->CurrentOptionNumber * 40 + data->Scene.ShakingY);
	}

	//팝업 출력
	if (!data->isShowingPopUp) 
	{//페이드 인 페이드 아웃 효과를 위한 검은색 배경의 투명도를 이용
		Renderer_DrawImage(&data->BlackOutImage, data->Scene.ShakingX, data->Scene.ShakingY);
	}
	else if (!data->isSceneChanging) 
	{//배경 전환중이 아니라면 팝업을 위로
		Renderer_DrawImage(&data->BlackOutImage, data->Scene.ShakingX, data->Scene.ShakingY);
		Renderer_DrawImage(&data->PopupImage, 0, 0);
	}
	else 
	{ //배경 전환중이면 검은 화면이 위로
		Renderer_DrawImage(&data->PopupImage, 0, 0);
		Renderer_DrawImage(&data->BlackOutImage, data->Scene.ShakingX, data->Scene.ShakingY);
	}

}

void release_main(void)
{
	MainScene* data = (MainScene*)g_Scene.Data;

	//씬 정보
	Scene_Clear(&data->Scene);

	//씬 전환
	Image_FreeImage(&data->BlackOutImage);

	//배경
	data->CurrentBGImage = NULL;
	Image_FreeImage(&data->UiImage);
	Image_FreeImage(&data->PopupImage);

	//텍스트
	Text_FreeText(&data->NullText);
	data->ShowText = NULL;

	//선택지
	Image_FreeImage(&data->OptionPointImage);

	SafeFree(g_Scene.Data);
}

#pragma endregion

#pragma region EndScene
typedef struct EndSceneData
{
	//배경
	Image   TitleImage;					//엔딩 모음집 배경 이미지(옆에 태두리)
	Text    Gototitle;					//엔딩 모음집 컨트롤 설명 텍스트

	//bgm
	Music   TitleBGM;					//엔딩 모음집 bgm
	float   Volume;						//bgm 볼륨

	//씬
	Text	SeeEnding[17];				//해당 엔딩을 본 경우 해당 엔딩 제목 표시
	int32   FontSize;					//엔딩 씬 부분 텍스트 크기

	//씬 이동
	int32   gotoscene[17];				//씬을 볼 곳
	int32	CurrentOptionNumber;		//메인으로 갈 시 해당 엔딩 신으로 갈 수 있게해줌 

	//포인터
	Image   OptionPointImage;			//엔딩씬 가리키는 포인터 이미지
	int32	CurrentOptionNumberX;		//포인터 이미지의 위치값(X)
	int32	CurrentOptionNumberY;		//포인터 이미지의 위치값(Y)

} EndSceneData;

void init_end(void)
{
	//기본 데이터 불러오기
	g_Scene.Data = malloc(sizeof(EndSceneData));
	memset(g_Scene.Data, 0, sizeof(EndSceneData));

	EndSceneData* data = (EndSceneData*)g_Scene.Data;

	//####배경
	Image_LoadImage(&data->TitleImage, "end.png");
	Text_CreateText(&data->Gototitle, TextFont, 20, L"ESC - 타이틀로                                          ENTER - 회상\0",
		wcslen(L"BACKSPACE - 타이틀로                                          ENTER - 회상\0")); //엔딩 모음집 컨트롤 설명


	//####bgm
	Audio_LoadMusic(&data->TitleBGM, "title.mp3");
	Audio_PlayFadeIn(&data->TitleBGM, INFINITY_LOOP, 2000);//bgm 플래이
	data->Volume = 1.0f;

	//####씬
	data->FontSize = 25;
	//##씬 이름, 이동할 위치 변경
	CsvFile csv2;
	memset(&csv2, 0, sizeof(CsvFile));
	CreateCsvFile(&csv2, CSV_FILE_NAME);
	for (int32 j = 0; j < 17; j++)
	{
		int32 goToSceneTemp = IsThisEndingShown(j + 121);

		if (goToSceneTemp != -1) //해당 엔딩을 봄
		{
			wchar_t* SceneNameTemp2 = ParseToUnicode(csv2.Items[j + 121][1]);
			data->gotoscene[j] = goToSceneTemp;

			Text_CreateText(&data->SeeEnding[j], TextFont, 25, SceneNameTemp2, wcslen(SceneNameTemp2));
		}
		else //해당 엔딩을 보지 못함
		{
			data->gotoscene[j] = -1;
			Text_CreateText(&data->SeeEnding[j], TextFont, data->FontSize, L"???????????", 13);
		}
	}
	FreeCsvFile(&csv2);

	//####포인터 관련
	data->CurrentOptionNumberX = 0; //포인터 위치, 다음 씬을 담을 위치 초기화
	data->CurrentOptionNumberY = 0;
	data->CurrentOptionNumber = 0;	
	Image_LoadImage(&data->OptionPointImage, ARROW_IMAGE_FILE); //포인터 이미지 추가
	data->OptionPointImage.ScaleX = 0.02f;
	data->OptionPointImage.ScaleY = 0.02f;

}

void update_end(void)
{
	//기본 데이터 가져오기
	EndSceneData* data = (EndSceneData*)g_Scene.Data;

	//키 입력으로 회귀할 옵션 넘버 지정
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

	//엔터 누르면 지정된 씬을 회귀함
	if (Input_GetKeyDown(VK_RETURN))
	{
		if (data->gotoscene[data->CurrentOptionNumber] != -1)
		{
			s_CurrentScene = data->gotoscene[data->CurrentOptionNumber];
			s_IsEndingScene = true;
			Scene_SetNextScene(SCENE_MAIN);
			Audio_Stop();
		}
	}

	//esc 누르면 타이틀로 나가기
	if (Input_GetKeyDown(VK_ESCAPE))
	{
		s_CurrentScene = 1;
		Scene_SetNextScene(SCENE_TITLE);
		Audio_Stop();
	}

}

void render_end(void)
{
	//기본 데이터 받아옴
	EndSceneData* data = (EndSceneData*)g_Scene.Data;

	//텍스트 컬러 지정
	SDL_Color color1 = { 0, 0, 0, 255 };	//배경 컬러
	SDL_Color Textcolor = { 255, 255, 255, 255 };	//텍스트 컬러

	//####배경
	Renderer_DrawImage(&data->TitleImage, 0, 0);
	Renderer_DrawTextShaded(&data->Gototitle, 1020, 840, Textcolor, color1);


	//####옵션 출력
	for (int i = 0; i < 16; ++i)
	{
		if (i < 8)
		{
			Renderer_DrawTextShaded(&data->SeeEnding[i], 100, 50 + i * 100, Textcolor, color1);
		}

		if (i >= 8)
		{
			Renderer_DrawTextShaded(&data->SeeEnding[i], 850, 50 + (i - 8) * 100, Textcolor, color1);
		}
		Renderer_DrawImage(&data->OptionPointImage, 50 + data->CurrentOptionNumberX, 50 + data->CurrentOptionNumberY * 100);
	}
}

void release_end(void)
{
	//기본 데이터 가져오기
	EndSceneData* data = (EndSceneData*)g_Scene.Data;

	//####배경
	Image_FreeImage(&data->TitleImage);
	Text_FreeText(&data->Gototitle);


	//####BGM
	Audio_FreeMusic(&data->TitleBGM);


	//####씬 이름 텍스트
	for (int32 i = 0; i < 17; i++) 
	{
		Text_FreeText(&data->SeeEnding[i]);
	}

	//####포인터
	Image_FreeImage(&data->OptionPointImage);


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
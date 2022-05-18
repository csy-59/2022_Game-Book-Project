#include "stdafx.h"
#include "Scene.h"
#include "Csv.h"
#include "Framework.h"
#include <crtdbg.h>
Scene g_Scene;

static ESceneType s_nextScene = SCENE_NULL;

#pragma region TitleScene
typedef struct TitleSceneData
{
	Text	EnterText;
	int32	FontSize;
	Music	TitleBGM;
	float	Volume;
	Image	TitleImage;
	int32	X;
	int32	Y;
	Image	StartImage;
	int32	SX;
	int32	SY;
} TitleSceneData;


void init_title(void)
{
	g_Scene.Data = malloc(sizeof(TitleSceneData));
	memset(g_Scene.Data, 0, sizeof(TitleSceneData));
	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;

	data->FontSize = 48;
	Text_CreateText(&data->EnterText, "chosun.ttf", data->FontSize, L"--- PRESS THE ENTER ---", 24);

	Image_LoadImage(&data->TitleImage, "title_2.jpg");
	data->X = 0;
	data->Y = 0;
	Image_LoadImage(&data->StartImage, "start.png");
	data->SX = (WINDOW_WIDTH / 2) - (data->StartImage.Width / 2);
	data->SY = 650;

	Audio_LoadMusic(&data->TitleBGM, "title.mp3");
	Audio_PlayFadeIn(&data->TitleBGM, INFINITY_LOOP, 3000);

	data->Volume = 1.0f;
}

void update_title(void)
{
	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;

	if (Input_GetKeyDown(VK_RETURN))
	{
		Scene_SetNextScene(SCENE_CREDIT);
	}
}

void render_title(void)
{
	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;

	Renderer_DrawImage(&data->TitleImage, data->X, data->Y);
	static float elapsedTime = 1.0f;

	SDL_Color color = { .a = 255 };
	elapsedTime += Timer_GetDeltaTime();

	if (elapsedTime >= 1.0f )
	{
		Renderer_DrawImage(&data->StartImage, data->SX, data->SY);
		Renderer_DrawTextSolid(&data->EnterText, WINDOW_WIDTH / 2 - (data->EnterText.Length * data->FontSize) / 4, 950, color);

		if (elapsedTime >= 1.5f)
		{
			elapsedTime = 0.0f;
		}
	}
}

void release_title(void)
{
	TitleSceneData* data = (TitleSceneData*)g_Scene.Data;

	Text_FreeText(&data->EnterText);
	Image_FreeImage(&data->TitleImage);
	Image_FreeImage(&data->StartImage);
	SafeFree(g_Scene.Data);
}
#pragma endregion


#pragma region CreditScene
typedef struct CreditSceneData
{
	Text	EnterText;
	int32	FontSize;
	TitleSceneData TitleBGM;
	Image	CreditImage;
	int32	X;
	int32	Y;
} CreditSceneData;

void init_credit(void)
{
	g_Scene.Data = malloc(sizeof(CreditSceneData));
	memset(g_Scene.Data, 0, sizeof(CreditSceneData));

	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;

	data->FontSize = 48;
	Text_CreateText(&data->EnterText, "chosun.ttf", data->FontSize, L"--- PRESS THE ENTER ---", 24);

	Image_LoadImage(&data->CreditImage, "credit.jpg");
	data->X = 0;
	data->Y = 0;
}

void update_credit(void)
{
	CreditSceneData* data = (CreditSceneData*)g_Scene.Data;

	if (Input_GetKeyDown(VK_RETURN))
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
	Image_FreeImage(&data->CreditImage);
	Audio_FreeMusic(&data->TitleBGM);
	
	SafeFree(g_Scene.Data);
}
#pragma endregion

#pragma region Intro

const wchar_t* introstr1[] = {
	L"한차례 퍼진 '좀비 병'의 유행으로 국가 멸망위기 까지 갔던 대한민국은, ",
	L"선진국들의 도움으로 '좀비 바이러스'를 퇴치하고 국가 정상화에 돌입했다.",
	L" ",
	L"더 이상의 '좀비병'으로 인한 나라의 멸망을 두고 볼 수 없던 대통령은",
	L" 대통령 산하 TF팀 <행복 연구소> 를 설립했다.",
};
const wchar_t* introstr2[] = {
	L"대통령 산하 TF팀 <행복 연구소> 에서는 ",
	L"국민 여러분의 행복과 안보를 책임질 여러 연구를 하고 있습니다.",
	L" ",
	L"여러분들의 건강과 행복을 ",
	L"질병과 바이러스로부터지켜드리기 위하여 ",
	L" ",
	L"<행복 연구소> 는 정부 기관 아래 최첨단 연구 장비와 지식을 겸비하여",
	L"꾸준한 연구 및 개발을 진행하고 있습니다.",
};
const wchar_t* introstr3[] = {
	L"하지만 사실, 이 <행복 연구소> 는 ",
	L"<좀비 바이러스를 이용한 대량 살상 무기>를 개발하는 곳이었고,  ",
	L"거기서 과학계에서 금지된 인간의 염색체를 ",
	L"이용한 지능 쥐 실험을 하고 있었다. 물론, 극비 사항이었다.",
	L" ",
	L"인간의 염색체와 뇌세포를 이식하여 지능을 높이고 ",
	L"최첨단 연구 장비와 지식을 겸비하여 ",
	L"꾸준한 연구 및 개발을 진행하고 있습니다. ",
};
const wchar_t* introstr4[] = {
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
const wchar_t* introstr5[] = {
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
const wchar_t* introstr6[] = {
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
	Image	Intro1Image;
	Image	Intro2Image;
	int32	X;
	int32	Y;
	Music	IntroBGM;
	float   Volume;

} IntroSceneData;
#define TextFont "GmarketSansTTFBold.ttf"

int count;

void init_intro(void)
{
	count = 1;
	g_Scene.Data = malloc(sizeof(IntroSceneData));
	memset(g_Scene.Data, 0, sizeof(IntroSceneData));

	IntroSceneData* data = (IntroSceneData*)g_Scene.Data;
	data->FontSize = 25;
	for (int32 i = 0; i < 5; ++i)
	{
		Text_CreateText(&data->Inro1Line[i], TextFont, data->FontSize, introstr1[i], wcslen(introstr1[i]));
	}
	for (int32 i = 0; i < 8; ++i)
	{
		Text_CreateText(&data->Inro2Line[i], TextFont, data->FontSize, introstr2[i], wcslen(introstr2[i]));
	}
	for (int32 i = 0; i < 8; ++i)
	{
		Text_CreateText(&data->Inro3Line[i], TextFont, data->FontSize, introstr3[i], wcslen(introstr3[i]));
	}
	for (int32 i = 0; i < 12; ++i)
	{
		Text_CreateText(&data->Inro4Line[i], TextFont, data->FontSize, introstr4[i], wcslen(introstr4[i]));
	}
	for (int32 i = 0; i < 10; ++i)
	{
		Text_CreateText(&data->Inro5Line[i], TextFont, data->FontSize, introstr5[i], wcslen(introstr5[i]));
	}
	for (int32 i = 0; i < 10; ++i)
	{
		Text_CreateText(&data->Inro6Line[i], TextFont, data->FontSize, introstr6[i], wcslen(introstr6[i]));
	}

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
	if (Input_GetKeyDown(VK_RETURN))
	{
		count++;
		if (count == 7)
		{
			Scene_SetNextScene(SCENE_MAIN);
		}
	}

}
#define INTRO_X 430
#define INTRO_Y 220
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
			Renderer_DrawTextSolid(&data->Inro1Line[i], INTRO_X, INTRO_Y + (50 * i), color);
		}
	}
	if (count == 2)
	{
		for (int32 i = 0; i < 8; ++i)
		{
			//Text_SetFontStyle(&data->Inro2Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro2Line[i], INTRO_X, INTRO_Y + (50 * i), color);
		}
	}
	if (count == 3)
	{
		for (int32 i = 0; i < 8; ++i)
		{
			//Text_SetFontStyle(&data->Inro3Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro3Line[i], INTRO_X, INTRO_Y + (50 * i), color);
		}
	}
	if (count == 4)
	{
		for (int32 i = 0; i < 12; ++i)
		{
			//Text_SetFontStyle(&data->Inro4Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro4Line[i], INTRO_X, INTRO_Y + (50 * i), color);
		}
	}
	if (count == 5)
	{
		for (int32 i = 0; i < 10; ++i)
		{
			//Text_SetFontStyle(&data->Inro5Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro5Line[i], INTRO_X, INTRO_Y + (50 * i), color);
		}
	}
	if (count == 6)
	{
		for (int32 i = 0; i < 10; ++i)
		{
			//Text_SetFontStyle(&data->Inro6Line[i], FS_BOLD);
			Renderer_DrawTextSolid(&data->Inro6Line[i], INTRO_X, INTRO_Y + (50 * i), color);
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
	Image_FreeImage(&data->Intro1Image);
	Image_FreeImage(&data->Intro2Image);
	Audio_FreeMusic(&data->IntroBGM);
	SafeFree(g_Scene.Data);
}
#pragma endregion

#pragma region MainScene

void logOnFinished(void)
{
	LogInfo("You can show this log on stopped the music");
}

void log2OnFinished(int32 channel)
{
	LogInfo("You can show this log on stopped the effect");
}

/*
void setScene(int32 indexNum, wchar_t* name, char* bgImageName,
	char* musicName, char* additionImageName, char* additionImagePos,
	char* effectSoundName, int32 effectSoundTime, int32 dialogCount,
	wchar_t** dialog, int32 optionCount, char* optionName,
	int32* nextSceneNumber, bool isEndingScene) {

	Scenes[indexNum-1].Number = indexNum-1;
	Scenes[indexNum-1].Name = name;
	Image_LoadImage(&Scenes[indexNum - 1].BGImage, bgImageName);
	Scenes[indexNum - 1].BGImage.ScaleY -= 0.065f;
	Scenes[indexNum - 1].BGImage.ScaleX -= 0.065f;
	Audio_LoadMusic(&Scenes[indexNum - 1].BGM, musicName);
	Image_LoadImage(&Scenes[indexNum - 1].AdditionImage, additionImageName);
	//���� �ʿ�
	Scenes[indexNum - 1].AddImage_X = 0;
	Scenes[indexNum - 1].AddImage_Y = 0;
	Audio_LoadMusic(&Scenes[indexNum - 1].EffectSound, effectSoundName);
	Scenes[indexNum - 1].EffectSoundTiming = effectSoundTime;

	Scenes[indexNum - 1].DialogCount = dialogCount;
	int32 i = 0, j = 0;
	for (int32 i = 0; i < dialogCount;i++) {
		while (wcslen(&dialog[i][j]) != 0) {
			Text_CreateText(&Scenes[indexNum-1].DialogList[i][j], )
		}
	}

	Text_CreateText(&Scenes[indexNum-1].DialogList[0][0], "d2coding.ttf", 50, L"2031�� 5�� 13�� �������� �������� ���� ", wcslen(L"2031�� 5�� 13�� �������� �������� ���� "));
	Text_CreateText(&Scenes[indexNum-1].DialogList[0][1], "d2coding.ttf", 50, L"��� ������ũ ���� �Դ�.", wcslen(L"��� ������ũ ���� �Դ�."));
	Text_CreateText(&Scenes[indexNum-1].DialogList[0][2], "d2coding.ttf", 50, L"", wcslen(L""));
	Text_CreateText(&Scenes[indexNum-1].DialogList[1][0], "d2coding.ttf", 50, L"�̰��� �������� ���� �����ϴ� ", wcslen(L"�̰��� �������� ���� �����ϴ� "));
	Text_CreateText(&Scenes[indexNum-1].DialogList[1][1], "d2coding.ttf", 50, L"Ű��ī�䰡 �ֱ� �����̴�.", wcslen(L"Ű��ī�䰡 �ֱ� �����̴�."));
	Text_CreateText(&Scenes[indexNum-1].DialogList[1][2], "d2coding.ttf", 50, L"", wcslen(L""));

	Scenes[indexNum-1].OptionCount = 0;
	Scenes[indexNum-1].NextSceneNumberList[0] = 1;
}
*/

#pragma endregion

#pragma region TempScene

typedef struct mainstruct
{	
	int32			Number;						//씬 넘버
	Image			BGImage;					//배경화면
	Music			BGM;						//배경 음악
	Text			DialogList[5];				//텍스트 배열
	int32			TextX;								
	int32			TextY;
	int32			NextSceneNumberList[6];			//옵션 선택시 넘어가는 씬 넘버
	int32			NextEndingSceneNumberList[6];	//다음 씬이 엔딩씬일 경우

} Mainscene;

#define SCENE_COUNT	8

Mainscene Scenes[SCENE_COUNT];


Text* ShowText;
int32 s_CurrentScene = 1;
SDL_Color color = { 0,0,0,0 };
bool isSceneChanging = false;
void init_main(void)
{
	
	
	g_Scene.Data = malloc(sizeof(Mainscene));
	memset(g_Scene.Data, 0, sizeof(Mainscene));
	Mainscene* data = (Mainscene*)g_Scene.Data;
	




}

void update_main(void)
{
	Mainscene* data = (Mainscene*)g_Scene.Data;

	//보통 씬일 경우
	if (!isSceneChanging)
	{
		
		
			
	
	
			if (Input_GetKeyDown(VK_RETURN)) 
			{
				isSceneChanging = true;
				data->Number++;
				Audio_FadeOut(1800);
			}
		}
	isSceneChanging = false;
}

void render_main(void)
{
	Mainscene* data = (Mainscene*)g_Scene.Data;

	//배경 이미지 출력
	//Renderer_DrawImage(&data->Scene->BGImage, 0, 0);
	//Renderer_DrawImage(&TextBGImage, 30, 30);
	Renderer_DrawImage(&data->BGImage, 0, 0);

	//텍스트 출력
	//int32 i = 0;
	//while (ShowText[i].Length != 0) 
	//{
	//	Renderer_DrawTextSolid(&ShowText[i], 400, 300 + i * 80, color);
	//	i++;
	//}
	//
	////선택지 출력
	//if (showOptions) {
	//	if (data->Scene->OptionCount > 2) {
	//		for (int i = 0; i < data->Scene->OptionCount; i++) {
	//			Renderer_DrawImage(&data->Scene->OptionImagesList[i], 250 + i % 2 * 700, 600 + (i / 2) * 200);
	//		}
	//	}
	//	else {
	//		for (int i = 0; i < data->Scene->OptionCount; i++) {
	//			Renderer_DrawImage(&data->Scene->OptionImagesList[i], 250 + i % 2 * 700, 700 + (i / 2) * 200);
	//		}
	//	}
	//}

}

void release_main(void)
{
	Mainscene* data = (Mainscene*)g_Scene.Data;

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
		//case SCENE_TEMP:
		//	g_Scene.Init = init_temp;
		//	g_Scene.Update = update_temp;
		//	g_Scene.Render = render_temp;
		//	g_Scene.Release = release_temp;
		//	break;
	}

	g_Scene.Init();

	s_nextScene = SCENE_NULL;
}
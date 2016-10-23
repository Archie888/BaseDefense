#include ".\game.h"
#include "main.h"

BackgroundObjectFactory::BackgroundObjectFactory(void)
{
}

BackgroundObjectFactory::~BackgroundObjectFactory(void)
{
}

GameObject * BackgroundObjectFactory::makeBackgroundObject(BackgroundObject::BACKGROUNDOBJECT_TYPE bgObjectType, Location location, Size size)
{
	if (bgObjectType >= BackgroundObject::BACKGROUNDOBJECT_TYPE::BG_TREE1 && bgObjectType < BackgroundObject::BACKGROUNDOBJECT_TYPE::BG_TREE_MAX)
		return new Tree((BackgroundObject::BACKGROUNDOBJECT_TYPE)bgObjectType, location, size);
	else if (bgObjectType >= BackgroundObject::BACKGROUNDOBJECT_TYPE::BG_BUSH1 && bgObjectType < BackgroundObject::BACKGROUNDOBJECT_TYPE::BG_BUSH_MAX)
		return new Bush((BackgroundObject::BACKGROUNDOBJECT_TYPE)bgObjectType, location, size);
	else return NULL;
}


SlideShow::SlideShow(void)
{
	images_showing = false;
	currentImage = NULL;
	setCurrentImageIndex(-1);

	for (int i = 0; i < MAX_NUMBER_OF_IMAGES; i++)
	{
		images[i] = NULL;
	}
}

SlideShow::~SlideShow(void)
{
}

bool SlideShow::play()
{
	controlMedia();
	startRendering();
	renderBackground();
	renderImages();
	renderText();
	stopRendering();

	return playing;
}

void SlideShow::controlMedia(void)
{
	timerDelta = hge->Timer_GetDelta();
	sceneTimer -= timerDelta;

	//audio
	{
		musicStartTimer -= timerDelta;

		if (musicStartTimer < 0.0f)
		{
			if (!hge->Channel_IsPlaying(musicChannel))
			{
				musicChannel = game.playMusic(music, 100, false);
				musicStartTimer = FLT_MAX;
			}
		}
	}

	//images
	{
		imageTimer -= timerDelta;

		if (imageTimer < 0.0f && getCurrentImageIndex() < nImages)
		{
			
			setCurrentImageIndex(getCurrentImageIndex() + 1);
			if (getCurrentImageIndex() < nImages)
			{
				currentImage = images[getCurrentImageIndex()];
				imageTimer = imageTimes[getCurrentImageIndex()];
			}
		}

		//end
		if (sceneTimer < 0.0f)
		{
			playing = false;
		}

		controlImages();

	}

	//text
	{
		controlText();
	}
}

void SlideShow::startRendering(void)
{
	hge->Gfx_BeginScene();
}

void SlideShow::stopRendering(void)
{
	hge->Gfx_EndScene();
}

void SlideShow::renderBackground(void)
{
	hge->Gfx_Clear(0);
}

void SlideShow::renderBoundedString(hgeFont * fnt, float x, float y, int align, float width, int nRenderedChars, const char * fmt, ...) {
   static char txt[1000];
   txt[0] = 0;
   
   // Print everything onto a temporary buffer:
   va_list va;
   va_start(va, fmt);
   vsprintf(txt, fmt, va);
   va_end(va);
   
   // Account for font scaling:
   width /= fnt->GetScale();
   
   // Transfer to a temporary string with line breaks to keep it within the
   // specified width:
   static std::string strFinal, strWord;
   strFinal.clear();
   strWord.clear();
   size_t len = strlen(txt);
   float currLineWidth = 0;
   float currWordWidth = 0;

   for(size_t i = 0; i <= 1000; i++) {
      //Calculate this character's width:
      float charWidth = 0;
      if(fnt->GetSprite(txt[i]))
         charWidth += fnt->GetSprite(txt[i])->GetWidth() + fnt->GetTracking();
      else if(fnt->GetSprite('?'))
         charWidth += fnt->GetSprite('?')->GetWidth() + fnt->GetTracking();
      
      if(txt[i] == ' ' || txt[i] == '\n' || txt[i] == '\0' || txt[i] == '~') {
         // Word delimiter found.
         // Word too big for the remaining space on this line?
         if(currLineWidth + currWordWidth - fnt->GetTracking() > width) {
            // Eliminate trailing spaces:
            while(strFinal[strFinal.size() - 1] == ' ')
               strFinal.resize(strFinal.size() - 1);
            
            // Break the line:
            strFinal += '\n';
            currLineWidth = 0;
            
            // Start new line with that word:
            strFinal += strWord;
            strFinal += txt[i];
            currLineWidth += currWordWidth;
            currLineWidth += charWidth;
         }
         else {
            // Word fits within this line.
            strFinal += strWord;
            strFinal += txt[i];
            currLineWidth += currWordWidth;
            currLineWidth += charWidth;
         }
         
         // Start new word:
         strWord.clear();
         currWordWidth = 0;
         
         // Delimiter was a line break?
         if(txt[i] == '\n'|| txt[i] == '~') {
            // Eliminate trailing spaces:
            while(strFinal[strFinal.size() - 1] == ' ')
               strFinal.resize(strFinal.size() - 1);
            
            // Start new line.
            currLineWidth = 0;
         }
      }
      else {
         // Word will become too big for the line width?
         if(currWordWidth + charWidth - fnt->GetTracking() > width) {
            // Start a new line with the current word fragment:
            if(currLineWidth > 0) {
               // Eliminate trailing spaces:
               while(strFinal[strFinal.size() - 1] == ' ')
                  strFinal.resize(strFinal.size() - 1);
               
               // Break the line:
               strFinal += '\n';
            }
            strFinal += strWord;
            strFinal += '\n';
            currLineWidth = 0;
            
            // Reset word:
            strWord.clear();
            currWordWidth = 0;
         }
         
         // Add to word:
         strWord += txt[i];
         currWordWidth += charWidth;
      }
   }

   if (nLetters == 297)
	   int u = 2;
   std::string strSub = strFinal.substr(0, nLetters);
   char *mopo = const_cast<char *>(strSub.c_str());


   int pos = strSub.find('~', 0); 
   while (pos != string::npos)
   {
		strSub.replace(pos, 1, "\n");
		pos = strSub.find('~', 0); 
   }

   fnt->Render(x, y, strSub.c_str(), align);
}


//intro data:
#define INTRO_TIME 78.0f
#define IMAGE_GHANGE_TIME_ACTION_SEQUENCE 3.18f
#define IMAGE_GHANGE_TIME_SORROW_SEQUENCE 7.0f
#define INTRO_IMAGES_START_TIME 3.0f
#define INTRO_MUSIC_START_TIME 4.0f
#define INTRO_IMAGES_FADE_TIME 1.0f 

//image box:
#define IMAGE_AREA_UPPER_LEFT_CORNER_X 50
#define IMAGE_AREA_UPPER_LEFT_CORNER_Y 50
#define IMAGE_AREA_WIDTH 500
#define IMAGE_AREA_HEIGHT 500
#define IMAGE_AREA_LOWER_RIGHT_CORNER_X IMAGE_AREA_UPPER_LEFT_CORNER_X + IMAGE_AREA_WIDTH
#define IMAGE_AREA_LOWER_RIGHT_CORNER_Y IMAGE_AREA_UPPER_LEFT_CORNER_Y + IMAGE_AREA_HEIGHT
#define IMAGE_AREA_CENTER_X (IMAGE_AREA_UPPER_LEFT_CORNER_X + IMAGE_AREA_WIDTH) / 2
#define IMAGE_AREA_CENTER_Y (IMAGE_AREA_UPPER_LEFT_CORNER_Y + IMAGE_AREA_HEIGHT) / 2
#define IMAGE_AREA_BACKGROUND_COLOR 0xff111111

//text box:
#define TEXT_AREA_LEFT_UPPER_CORNER_X 540
#define TEXT_AREA_LEFT_UPPER_CORNER_Y 150
#define TEXT_AREA_WIDTH 300
#define TEXT_AREA_HEIGHT 300
#define TEXT_AREA_RIGHT_LOWER_CORNER_X TEXT_AREA_LEFT_UPPER_CORNER_X + TEXT_AREA_WIDTH
#define TEXT_AREA_RIGHT_LOWER_CORNER_Y TEXT_AREA_LEFT_UPPER_CORNER_Y + TEXT_AREA_HEIGHT
#define TEXT_AREA_BACKGROUND_COLOR 0xff111111
#define TEXT_FADING_AREA_LENGTH 100

//text:
#define INTRO_TEXT_LEFT_SPACING 50
#define INTRO_TEXT_START_UPPER_SPACING TEXT_AREA_HEIGHT / 2
#define INTRO_TEXT_LETTER_APPEARANCE_SPEED 0.035f
#define INTRO_TEXT_COMMA_PAUSE_TIME 0.5f
#define INTRO_TEXT_DOT_PAUSE_TIME 0.8f
#define INTRO_TEXT_SLIDING_SPEED 30.0f
#define INTRO_TEXT_START_TIME INTRO_MUSIC_START_TIME + 16.0f

//map zoom effect:
#define MAP_ZOOM_FIRST_IMAGE_INDEX 3
#define MAP_ZOOM_IMAGE_1 3
#define MAP_ZOOM_IMAGE_2 4
#define MAP_ZOOM_SPEED 0.73f 
#define MAP_ZOOM_FLIPS 2
#define MAP_ZOOM_FLIP_TIME 0.1f
#define START_SCALE_IMAGE_1 0.496896f
#define START_SCALE_IMAGE_2 1.211540f
#define START_SCALE_IMAGE_3 1.021003f
#define END_SCALE_IMAGE_1 1.033374f
#define END_SCALE_IMAGE_2 4.102918f
#define END_SCALE_IMAGE_3 10.0f //1.500000f
#define CENTER_X_IMAGE_1 400.624329f
#define CENTER_X_IMAGE_2 398.310547f
#define CENTER_X_IMAGE_3 401.208984f
#define CENTER_Y_IMAGE_1 556.976624f
#define CENTER_Y_IMAGE_2 267.800018f
#define CENTER_Y_IMAGE_3 241.899719f

//'action' sequence
#define ACTION_SEQUENCE_FIRST_INDEX 2
#define ACTION_SEQUENCE_LAST_INDEX 10

//'sorrow' sequence
#define SORROW_SEQUENCE_INDEX_1 11
#define SORROW_SEQUENCE_INDEX_2 12
#define SORROW_SEQUENCE_INDEX_3 13
#define SORROW_SEQUENCE_INDEX_4 14

Intro::Intro(void)
{
	//init sounds
	{
		music = game.sounds[Game::SOUND_INTRO_MUSIC];
		musicStartTimer = INTRO_MUSIC_START_TIME;
	}

	//init images
	{
		nImages = NUMBER_OF_IMAGES;
		for (int i = 0; i < nImages; i++)
		{
			images[i] = game.animations[Game::ANIMATION_INTRO_IMAGE_1 + i];
			images[i]->SetHotSpot(images[i]->GetWidth() / 2.0f, images[i]->GetHeight() / 2.0f);
			imageScales[i] = 1.0f;

			if (i < SORROW_SEQUENCE_INDEX_1)
				imageTimes[i] = IMAGE_GHANGE_TIME_ACTION_SEQUENCE;
			else
				imageTimes[i] = IMAGE_GHANGE_TIME_SORROW_SEQUENCE;
		}

		//distant combat
		imageTimes[0] = 8.5f;
		imageScales[0] = 0.9f;
		imageTimes[1] = 8.4f;
		imageScales[1] = 0.9f;

		////militaryzone
		images[MAP_ZOOM_IMAGE_1]->SetHotSpot(CENTER_X_IMAGE_1, CENTER_Y_IMAGE_1);
		images[MAP_ZOOM_IMAGE_2]->SetHotSpot(CENTER_X_IMAGE_2, CENTER_Y_IMAGE_2);
		imageScales[MAP_ZOOM_IMAGE_1] = START_SCALE_IMAGE_1;
		imageScales[MAP_ZOOM_IMAGE_2] = START_SCALE_IMAGE_2;
		imageTimes[MAP_ZOOM_IMAGE_1] = FLT_MAX;
		imageTimes[MAP_ZOOM_IMAGE_2] = FLT_MAX;

		//sorrow
		imageScales[12] = 0.7f;

		imageTimer = INTRO_IMAGES_START_TIME;

		flips = MAP_ZOOM_FLIPS;
		flipTimer = 0.0f;

		imagebackgroundtex = hge->Texture_Create(Game::SCREENWIDTH, Game::SCREENHEIGHT);
		int w = hge->Texture_GetWidth(imagebackgroundtex);
		DWORD * colArray = hge->Texture_Lock(imagebackgroundtex);

		//create the background clipper (hge clipping function has bugs)
		for (int i = 0; i < Game::SCREENHEIGHT; i++)
		{
			for (int j = 0; j < Game::SCREENWIDTH; j++)
			{
				if (j > IMAGE_AREA_UPPER_LEFT_CORNER_X &&
					j < IMAGE_AREA_LOWER_RIGHT_CORNER_X &&
					i > IMAGE_AREA_UPPER_LEFT_CORNER_Y &&
					i < IMAGE_AREA_LOWER_RIGHT_CORNER_Y)
				{
					//set transparent
					colArray[i * w + j] = 0x00000000;
				}
				else
				{
					//set black
					colArray[i * w + j] = 0xff000000;
				}
			}
		}

		hge->Texture_Unlock(imagebackgroundtex);

		imageBackground = new hgeAnimation(imagebackgroundtex, 1, 0.0f, 0, 0, hge->Texture_GetWidth(imagebackgroundtex), hge->Texture_GetHeight(imagebackgroundtex));
	}

	//init text
	{
		hge->System_SetState(HGE_INIFILE, "Intro.ini");
		for (int i = 0; i < NUMBER_OF_LETTERS; i++)
		{
			text[i] = ' ';
		}
		int i = 1;
		char * storyTemp;
		std::string wholeStory;
		while (i < 255)
		{
			char stringName[255];
			sprintf(stringName, "STORY_%i", i);
			storyTemp = hge->Ini_GetString("STORY", stringName, NULL);

			if (storyTemp)
			{
				wholeStory += storyTemp;
			}
			else
				break;

			i++;
		}

		sprintf(text, wholeStory.c_str());

		letterTimer = 0.0f;
		textTimer = INTRO_TEXT_START_TIME;
		nLetters = 0;
		textX = INTRO_TEXT_LEFT_SPACING;
		textY = INTRO_TEXT_START_UPPER_SPACING;

		//text background and filter
		textbackgroundtex = hge->Texture_Create(800, 600);
		textshadowfiltertex = hge->Texture_Create(800, 600);
		int w = hge->Texture_GetWidth(textbackgroundtex);
		DWORD * colArray1 = hge->Texture_Lock(textbackgroundtex);
		DWORD * colArray2 = hge->Texture_Lock(textshadowfiltertex);

		//create the background clipper (hge clipping function has bugs)
		for (int i = 0; i < 600; i++)
		{
			for (int j = 0; j < 800; j++)
			{
				//text bg
				colArray1[i * w + j] = TEXT_AREA_BACKGROUND_COLOR;

				//text filter
				if (i < TEXT_FADING_AREA_LENGTH)
				{
					DWORD color = TEXT_AREA_BACKGROUND_COLOR;
					float floatShade = 255 * (1.0f - (float)i / (float)TEXT_FADING_AREA_LENGTH);
					int intShade = (int)floatShade;
					BYTE a, r, g, b;
					getColorComponents(color, &a, &r, &g, &b);
					color = getColor(intShade, r, g, b);

					colArray2[i * w + j] = color;
				}
				else
				{
					colArray2[i * w + j] = 0;
				}
			}
		}

		hge->Texture_Unlock(textbackgroundtex);
		hge->Texture_Unlock(textshadowfiltertex);
		
		textBackground = new hgeAnimation(textbackgroundtex, 1, 0.0f, 0, 0, hge->Texture_GetWidth(textbackgroundtex), hge->Texture_GetHeight(textbackgroundtex));
		textShadowFilterLayer = new hgeAnimation(textshadowfiltertex, 1, 0.0f, 0, 0, hge->Texture_GetWidth(textshadowfiltertex), hge->Texture_GetHeight(textshadowfiltertex));
	}

	sceneTimer = INTRO_TIME;
	playing = true;
}

Intro::~Intro(void)
{
	hge->Texture_Free(imagebackgroundtex);
	delete imageBackground;
}

void Intro::controlImages(void)
{
	int ind = getCurrentImageIndex();
	switch (ind)
	{
	case 0: 
		{
			//* fading effects *

			//fade in:
			float fadeInTime = 3.0f;
			if (imageTimer > imageTimes[getCurrentImageIndex()] - fadeInTime && currentImage != NULL)
			{
				float time = imageTimer - (imageTimes[getCurrentImageIndex()] - fadeInTime);
				float floatValue = 255.0f * (time / fadeInTime);
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);

				hge->System_SetState(HGE_INIFILE, "debuglog.ini");
				hge->Ini_SetInt("INTRO_DEBUG", "invalue", intValue);

			}

			//fade out:
			if (imageTimer < INTRO_IMAGES_FADE_TIME && currentImage != NULL)
			{
				float floatValue = 255.0f * (1.0f - (imageTimer / INTRO_IMAGES_FADE_TIME));
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);
			}

			break;
		}
	case 1:
		{
		if (getCurrentImageIndex() >= 0 && getCurrentImageIndex() < nImages)
			{
				//* fading effects *

				int i = getCurrentImageIndex();
				//fade in:
				if (imageTimer > imageTimes[getCurrentImageIndex()] - INTRO_IMAGES_FADE_TIME && currentImage != NULL)
				{
					float time = imageTimer - (imageTimes[getCurrentImageIndex()] - INTRO_IMAGES_FADE_TIME);
					float floatValue = 255.0f * (time / INTRO_IMAGES_FADE_TIME);
					int intValue = (int)floatValue;
					DWORD col = colorAdd(0xffffffff, -intValue);
					images[getCurrentImageIndex()]->SetColor(col);

					hge->System_SetState(HGE_INIFILE, "debuglog.ini");
					hge->Ini_SetInt("INTRO_DEBUG", "invalue", intValue);

				}

				//fade out:
				if (imageTimer < INTRO_IMAGES_FADE_TIME && currentImage != NULL)
				{
					float floatValue = 255.0f * (1.0f - (imageTimer / INTRO_IMAGES_FADE_TIME));
					int intValue = (int)floatValue;
					DWORD col = colorAdd(0xffffffff, -intValue);
					images[getCurrentImageIndex()]->SetColor(col);
				}
			}
			break;
		}
	case 2:
		{

			break;
		}
	case MAP_ZOOM_IMAGE_1:
		{
			if (imageScales[getCurrentImageIndex()] < END_SCALE_IMAGE_1)
				imageScales[getCurrentImageIndex()] += MAP_ZOOM_SPEED * imageScales[getCurrentImageIndex()] * timerDelta;
			else
			{
				flipTimer += timerDelta;

				if (flipTimer > MAP_ZOOM_FLIP_TIME)
				{
					flipTimer = 0.0f;
					imageTimer = 0.0f;
				}
			}
			break;
		}
	case MAP_ZOOM_IMAGE_2:
		{
			if (flips > 0)
			{
				flipTimer += timerDelta;

				if (flipTimer > MAP_ZOOM_FLIP_TIME)
				{
					flipTimer = 0.0f;
					setCurrentImageIndex(getCurrentImageIndex() - 1);
					currentImage = images[getCurrentImageIndex()];
					flips--;
				}				
			}
			else
			{
				if (imageScales[getCurrentImageIndex()] < END_SCALE_IMAGE_2)
					imageScales[getCurrentImageIndex()] += MAP_ZOOM_SPEED * imageScales[getCurrentImageIndex()] * timerDelta;
				else
				{
					imageTimer = 0.0f;
				}
			}

			break;
		}
	case ACTION_SEQUENCE_LAST_INDEX:
		{
			//fade out all:
			if (imageTimer < INTRO_IMAGES_FADE_TIME && currentImage != NULL)
			{
				float floatValue = 255.0f * (1.0f - (imageTimer / INTRO_IMAGES_FADE_TIME));
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);
				float val = 255 - intValue;
				textBackground->SetColor(getColor(val, val, val, val));
				textShadowFilterLayer->SetColor(getColor(val, val, val, val));
				fnt->SetColor(col);
			}
			break;
		}
	case SORROW_SEQUENCE_INDEX_1:
	case SORROW_SEQUENCE_INDEX_2:
	case SORROW_SEQUENCE_INDEX_3:
	case SORROW_SEQUENCE_INDEX_4:
		{
			//Reset font color:
			fnt->SetColor(0xffffffff);

			//* fading effects *

			int i = getCurrentImageIndex();
			//fade in:
			if (imageTimer > imageTimes[getCurrentImageIndex()] - INTRO_IMAGES_FADE_TIME && currentImage != NULL)
			{
				float time = imageTimer - (imageTimes[getCurrentImageIndex()] - INTRO_IMAGES_FADE_TIME);
				float floatValue = 255.0f * (time / INTRO_IMAGES_FADE_TIME);
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);

				hge->System_SetState(HGE_INIFILE, "debuglog.ini");
				hge->Ini_SetInt("INTRO_DEBUG", "invalue", intValue);

			}

			//fade out:
			if (imageTimer < INTRO_IMAGES_FADE_TIME && currentImage != NULL)
			{
				float floatValue = 255.0f * (1.0f - (imageTimer / INTRO_IMAGES_FADE_TIME));
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);
			}

			break;
		}
	default:
		{	


			break;
		}
	}
}

void Intro::controlText(void)
{
	textTimer -= timerDelta;

	if (textTimer < 0.0f)
	{	
		textY -= INTRO_TEXT_SLIDING_SPEED * timerDelta;
		letterTimer -= timerDelta;

		if (letterTimer < 0.0f)
		{
			if (nLetters < NUMBER_OF_LETTERS)
			{
				if (text[nLetters] == ',' && text[nLetters + 1] == ' ')
					letterTimer = INTRO_TEXT_COMMA_PAUSE_TIME;
				else if (text[nLetters] == '.' && text[nLetters + 1] == ' ')
					letterTimer = INTRO_TEXT_DOT_PAUSE_TIME;
				else
					letterTimer = INTRO_TEXT_LETTER_APPEARANCE_SPEED;

				nLetters++;
				//text[nLetters] = '\0';
			}

//				if (
//				letterTimer = INTRO_TEXT_LETTER_APPEARANCE_SPEED;
		}
	}
}

void Intro::renderImages(void)
{
	int ind = getCurrentImageIndex();
	
	//action sequence
    if (ind >= ACTION_SEQUENCE_FIRST_INDEX && ind <= ACTION_SEQUENCE_LAST_INDEX)
	{
		if (currentImage)
		{
			currentImage->RenderEx(IMAGE_AREA_CENTER_X, IMAGE_AREA_CENTER_Y, 0, imageScales[getCurrentImageIndex()], imageScales[getCurrentImageIndex()]);
			images_showing = true;
		}
		if (imageBackground)
		{
			imageBackground->RenderEx(0, 0, 0);
			images_showing = true;
		}
	}
	else
	{
		if (currentImage)
		{
			currentImage->RenderEx(Game::SCREENWIDTH / 2, Game::SCREENHEIGHT / 2, 0, imageScales[getCurrentImageIndex()], imageScales[getCurrentImageIndex()]);
			images_showing = true;
		}
	}
}

void Intro::renderText(void)
{
	int ind = getCurrentImageIndex();
	
	//action sequence
    if (ind >= ACTION_SEQUENCE_FIRST_INDEX && ind <= ACTION_SEQUENCE_LAST_INDEX)
	{
		hge->Gfx_SetClipping(TEXT_AREA_LEFT_UPPER_CORNER_X, TEXT_AREA_LEFT_UPPER_CORNER_Y, TEXT_AREA_WIDTH, TEXT_AREA_HEIGHT);
		textBackground->RenderEx(0, 0, 0);
		fnt->SetScale(1.3f);
		renderBoundedString(fnt, textX, textY, 20, 750, nLetters, text);
		textShadowFilterLayer->RenderEx(0, 0, 0);
		hge->Gfx_SetClipping();
	}
}


#define VICTORY_SCENE_TIME 37.0f
#define VICTORY_SCENE_TEXT_START_TIME 2.0f
#define VICTORY_SCENE_IMAGE_GHANGE_TIME 7.0f
#define VICTORY_SCENE_IMAGE_FADE_TIME 2.0f
#define VICTORY_SCENE_LAST_IMAGE_INDEX 3
#define VICTORY_SCENE_FINAL_ANNOUNCEMENT_INDEX 4
#define VICTORY_SCENE_FINAL_ANNOUNCEMENT "VICTORY!"
#define VICTORY_SCENE_FINAL_ANNOUNCEMENT_TIME 7.0f

//text
#define VICTORY_SCENE_TEXT_LETTER_APPEARANCE_SPEED 0.06f
#define VICTORY_SCENE_TEXT_SLIDING_SPEED 20.0f
#define VICTORY_SCENE_TEXT_COMMA_PAUSE_TIME 1.0f
#define VICTORY_SCENE_TEXT_DOT_PAUSE_TIME 2.0f

VictoryScene::VictoryScene(void)
{
	//init sounds
	{
		music = game.sounds[Game::SOUND_VICTORY_MUSIC_1];
		musicStartTimer = 0.0f;
	}

	//init images
	{
		nImages = NUMBER_OF_IMAGES + 1;
		for (int i = 0; i < nImages - 1; i++)
		{
			images[i] = game.animations[Game::ANIMATION_VICTORY_1 + i];
			images[i]->SetHotSpot(images[i]->GetWidth() / 2.0f, images[i]->GetHeight() / 2.0f);
			imageScales[i] = 1.0f;
			imageTimes[i] = VICTORY_SCENE_IMAGE_GHANGE_TIME;
		}
		imageTimes[VICTORY_SCENE_FINAL_ANNOUNCEMENT_INDEX] = VICTORY_SCENE_FINAL_ANNOUNCEMENT_TIME;

		imageTimer = 1.0f;

		imagebackgroundtex = hge->Texture_Create(Game::SCREENWIDTH, Game::SCREENHEIGHT);
		int w = hge->Texture_GetWidth(imagebackgroundtex);
		DWORD * colArray = hge->Texture_Lock(imagebackgroundtex);

		//create the background clipper (hge clipping function has bugs)
		for (int i = 0; i < Game::SCREENHEIGHT; i++)
		{
			for (int j = 0; j < Game::SCREENWIDTH; j++)
			{
				if (j > IMAGE_AREA_UPPER_LEFT_CORNER_X &&
					j < IMAGE_AREA_LOWER_RIGHT_CORNER_X &&
					i > IMAGE_AREA_UPPER_LEFT_CORNER_Y &&
					i < IMAGE_AREA_LOWER_RIGHT_CORNER_Y)
				{
					//set transparent
					colArray[i * w + j] = 0x00000000;
				}
				else
				{
					//set black
					colArray[i * w + j] = 0xff000000;
				}
			}
		}

		hge->Texture_Unlock(imagebackgroundtex);

		imageBackground = new hgeAnimation(imagebackgroundtex, 1, 0.0f, 0, 0, hge->Texture_GetWidth(imagebackgroundtex), hge->Texture_GetHeight(imagebackgroundtex));

	}

	//init text object
	{
		imageTimes[VICTORY_SCENE_FINAL_ANNOUNCEMENT_INDEX] = VICTORY_SCENE_FINAL_ANNOUNCEMENT_TIME;
		textObject = new hgeGUIText(0, 200, 150, 400, 200, fnt);
		textObject->SetText(VICTORY_SCENE_FINAL_ANNOUNCEMENT);
		textObject->SetMode(HGETEXT_CENTER);
	}

	//init text
	{
		hge->System_SetState(HGE_INIFILE, "Outro.ini");
		for (int i = 0; i < NUMBER_OF_LETTERS; i++)
		{
			text[i] = ' ';
		}

		int i = 1;

		char * storyTemp;
		std::string wholeStory;
		while (i < 255)
		{
			char stringName[255];
			sprintf(stringName, "STORY_%i", i);
			storyTemp = hge->Ini_GetString("STORY", stringName, NULL);

			if (storyTemp)
			{
				wholeStory += storyTemp;
			}
			else
				break;

			i++;
		}

		sprintf(text, wholeStory.c_str());

		letterTimer = 0.0f;
		textTimer = VICTORY_SCENE_TEXT_START_TIME;
		nLetters = 0;
		textX = INTRO_TEXT_LEFT_SPACING;
		textY = INTRO_TEXT_START_UPPER_SPACING;

		//text background and filter
		textbackgroundtex = hge->Texture_Create(800, 600);
		textshadowfiltertex = hge->Texture_Create(800, 600);
		int w = hge->Texture_GetWidth(textbackgroundtex);
		DWORD * colArray1 = hge->Texture_Lock(textbackgroundtex);
		DWORD * colArray2 = hge->Texture_Lock(textshadowfiltertex);

		//create the background clipper (hge clipping function has bugs)
		for (int i = 0; i < 600; i++)
		{
			for (int j = 0; j < 800; j++)
			{
				//text bg
				colArray1[i * w + j] = TEXT_AREA_BACKGROUND_COLOR;

				//text filter
				if (i < TEXT_FADING_AREA_LENGTH)
				{
					DWORD color = TEXT_AREA_BACKGROUND_COLOR;
					float floatShade = 255 * (1.0f - (float)i / (float)TEXT_FADING_AREA_LENGTH);
					int intShade = (int)floatShade;
					BYTE a, r, g, b;
					getColorComponents(color, &a, &r, &g, &b);
					color = getColor(intShade, r, g, b);

					colArray2[i * w + j] = color;
				}
				else
				{
					colArray2[i * w + j] = 0;
				}
			}
		}

		hge->Texture_Unlock(textbackgroundtex);
		hge->Texture_Unlock(textshadowfiltertex);
		
		textBackground = new hgeAnimation(textbackgroundtex, 1, 0.0f, 0, 0, hge->Texture_GetWidth(textbackgroundtex), hge->Texture_GetHeight(textbackgroundtex));
		textBackground->SetColor(0xff000000);
		textShadowFilterLayer = new hgeAnimation(textshadowfiltertex, 1, 0.0f, 0, 0, hge->Texture_GetWidth(textshadowfiltertex), hge->Texture_GetHeight(textshadowfiltertex));
	}

	sceneTimer = VICTORY_SCENE_TIME;
	playing = true;
}

VictoryScene::~VictoryScene(void)
{
	hge->Texture_Free(imagebackgroundtex);
	delete imageBackground;
}

void VictoryScene::controlImages(void)
{
	int ind = getCurrentImageIndex();
	switch (ind)
	{
	case 0:
		{
			//fade in:
			float fadeInTime = VICTORY_SCENE_IMAGE_FADE_TIME;
			if (imageTimer > imageTimes[getCurrentImageIndex()] - fadeInTime && currentImage != NULL)
			{
				float time = imageTimer - (imageTimes[getCurrentImageIndex()] - fadeInTime);
				float floatValue = 255.0f * (time / fadeInTime);
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);

				hge->System_SetState(HGE_INIFILE, "debuglog.ini");
				hge->Ini_SetInt("INTRO_DEBUG", "invalue", intValue);

				float val = 255 - intValue;
				textBackground->SetColor(getColor(val, val, val, val));
				textShadowFilterLayer->SetColor(getColor(val, val, val, val));

				fnt->SetColor(col);
			}

			//fade out:
			float fadeOutTime = VICTORY_SCENE_IMAGE_FADE_TIME;
			if (imageTimer < fadeOutTime && currentImage != NULL)
			{
				float floatValue = 255.0f * (1.0f - (imageTimer / fadeOutTime));
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);
			}
			break;
		}
	case 1:
	case 2:
		{
			//* fading effects *

			//fade in:
			float fadeInTime = VICTORY_SCENE_IMAGE_FADE_TIME;
			if (imageTimer > imageTimes[getCurrentImageIndex()] - fadeInTime && currentImage != NULL)
			{
				float time = imageTimer - (imageTimes[getCurrentImageIndex()] - fadeInTime);
				float floatValue = 255.0f * (time / fadeInTime);
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);

				hge->System_SetState(HGE_INIFILE, "debuglog.ini");
				hge->Ini_SetInt("INTRO_DEBUG", "invalue", intValue);

			}

			//fade out:
			float fadeOutTime = VICTORY_SCENE_IMAGE_FADE_TIME;
			if (imageTimer < fadeOutTime && currentImage != NULL)
			{
				float floatValue = 255.0f * (1.0f - (imageTimer / fadeOutTime));
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);
			}
			break;
		}
	case 3:
		{
			//fade out all:
			if (imageTimer < VICTORY_SCENE_IMAGE_FADE_TIME && currentImage != NULL)
			{
				float floatValue = 255.0f * (1.0f - (imageTimer / VICTORY_SCENE_IMAGE_FADE_TIME));
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				//int * p = (int *)col;
				//int u = 2;
				images[getCurrentImageIndex()]->SetColor(col);

				float val = 255 - intValue;
				textBackground->SetColor(getColor(val, val, val, val));
				textShadowFilterLayer->SetColor(getColor(val, val, val, val));

				fnt->SetColor(col);
			}
			break;
		}
	}
}

void VictoryScene::controlText(void)
{
	int ind = getCurrentImageIndex();
	
	textTimer -= timerDelta;

	if (ind < VICTORY_SCENE_FINAL_ANNOUNCEMENT_INDEX)
	{	
		if (textTimer < 0.0f)
		{	
			textY -= VICTORY_SCENE_TEXT_SLIDING_SPEED * timerDelta;
			letterTimer -= timerDelta;

			if (letterTimer < 0.0f)
			{
				if (nLetters < NUMBER_OF_LETTERS)
				{
					if (text[nLetters] == ',' && text[nLetters + 1] == ' ')
						letterTimer = VICTORY_SCENE_TEXT_COMMA_PAUSE_TIME;
					else if (text[nLetters] == '.' && text[nLetters + 1] == ' ')
						letterTimer = VICTORY_SCENE_TEXT_DOT_PAUSE_TIME;
					else
						letterTimer = VICTORY_SCENE_TEXT_LETTER_APPEARANCE_SPEED;

					nLetters++;
				}
			}
		}

	}

    if (ind == VICTORY_SCENE_FINAL_ANNOUNCEMENT_INDEX)
	{	
		//* fading effects *

		//fade in:
		float fadeInTime = VICTORY_SCENE_IMAGE_FADE_TIME;
		if (imageTimer > VICTORY_SCENE_FINAL_ANNOUNCEMENT_TIME - fadeInTime)
		{
			float time = imageTimer - (VICTORY_SCENE_FINAL_ANNOUNCEMENT_TIME - fadeInTime);
			float floatValue = 255.0f * (time / fadeInTime);
			int intValue = (int)floatValue;
			DWORD col = colorAdd(0xffffffff, -intValue);
			textObject->SetColor(col);
		}

		//fade out:
		float fadeOutTime = VICTORY_SCENE_IMAGE_FADE_TIME;
		if (imageTimer < fadeOutTime)
		{
			float floatValue = 255.0f * (1.0f - (imageTimer / fadeOutTime));
			int intValue = (int)floatValue;
			DWORD col = colorAdd(0xffffffff, -intValue);
			textObject->SetColor(col);
		}
	}
}

void VictoryScene::renderImages(void)
{
	int ind = getCurrentImageIndex();
	
	//action sequence
    if (ind >= 0 && ind <= VICTORY_SCENE_LAST_IMAGE_INDEX)
	{	
		if (currentImage)
		{
			currentImage->RenderEx(IMAGE_AREA_CENTER_X, IMAGE_AREA_CENTER_Y, 0, imageScales[getCurrentImageIndex()], imageScales[getCurrentImageIndex()]);
			images_showing = true;
		}
		if (imageBackground)
		{
			imageBackground->RenderEx(0, 0, 0);
			images_showing = true;
		}
	}
}

void VictoryScene::renderText(void)
{
	int ind = getCurrentImageIndex();
	
    if (ind >= 0 && ind <= VICTORY_SCENE_LAST_IMAGE_INDEX)
	{
		hge->Gfx_SetClipping(TEXT_AREA_LEFT_UPPER_CORNER_X, TEXT_AREA_LEFT_UPPER_CORNER_Y, TEXT_AREA_WIDTH, TEXT_AREA_HEIGHT);
		textBackground->RenderEx(0, 0, 0);
		fnt->SetScale(1.3f);
		renderBoundedString(fnt, textX, textY, 20, 750, nLetters, text);
		textShadowFilterLayer->RenderEx(0, 0, 0);
		hge->Gfx_SetClipping();
	}
    if (ind == VICTORY_SCENE_FINAL_ANNOUNCEMENT_INDEX)
	{
		fnt->SetScale(2.0f);
		if (textObject)
		{
			textObject->Render();
		}
	}
}


//Game over scene data:
#define GAME_OVER_SCENE_TIME 31.0f
#define GAME_OVER_SCENE_IMAGES_START_TIME 1.0f
#define GAME_OVER_SCENE_IMAGE_FADE_TIME 3.0f
#define GAME_OVER_SCENE_IMAGE_CHANGE_TIME 8.0f
#define GAME_OVER_SCENE_TEXT "GAME OVER"
#define GAME_OVER_SCENE_TEXT_IMAGE_INDEX 2
#define GAME_OVER_SCENE_TEXT_TIME 10.0f

GameOverScene::GameOverScene(void)
{
	//init sounds
	{
		music = game.sounds[Game::SOUND_GAME_OVER_MUSIC_1];
		musicStartTimer = 0.0f;
	}

	//init images
	{
		nImages = NUMBER_OF_IMAGES;
		for (int i = 0; i < nImages; i++)
		{
			images[i] = game.animations[Game::ANIMATION_GAME_OVER_1 + i];
			images[i]->SetHotSpot(images[i]->GetWidth() / 2.0f, images[i]->GetHeight() / 2.0f);
			imageScales[i] = 1.0f;
			imageTimes[i] = GAME_OVER_SCENE_IMAGE_CHANGE_TIME;
		}

		imageTimer = GAME_OVER_SCENE_IMAGES_START_TIME;

		hge->Gfx_BeginScene();
		for (int i = 0; i < nImages; i++)
		{
			while (!images[i]->GetTexture())
				; //fnt->printf(100, 100, "not getting image %i texture", i);
			images[i]->Render(-5000, -5000);
		}
		hge->Gfx_EndScene();

		nImages = 4;
	}

	//init text object
	{
		imageTimes[GAME_OVER_SCENE_TEXT_IMAGE_INDEX] = GAME_OVER_SCENE_TEXT_TIME;
		textObject = new hgeGUIText(0, 200, 150, 400, 200, fnt);
		textObject->SetText(GAME_OVER_SCENE_TEXT);
		textObject->SetMode(HGETEXT_CENTER);
	}

	sceneTimer = GAME_OVER_SCENE_TIME;
	playing = true;
}

GameOverScene::~GameOverScene(void)
{
}

void GameOverScene::controlImages(void)
{
	int ind = getCurrentImageIndex();
	switch (ind)
	{
	case 0: 
	case 1:
		{
			//* fading effects *

			//fade in:
			float fadeInTime = GAME_OVER_SCENE_IMAGE_FADE_TIME;
			if (imageTimer > imageTimes[getCurrentImageIndex()] - fadeInTime && currentImage != NULL)
			{
				float time = imageTimer - (imageTimes[getCurrentImageIndex()] - fadeInTime);
				float floatValue = 255.0f * (time / fadeInTime);
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);

				hge->System_SetState(HGE_INIFILE, "debuglog.ini");
				hge->Ini_SetInt("INTRO_DEBUG", "invalue", intValue);

			}

			//fade out:
			float fadeOutTime = GAME_OVER_SCENE_IMAGE_FADE_TIME;
			if (imageTimer < fadeOutTime && currentImage != NULL)
			{
				float floatValue = 255.0f * (1.0f - (imageTimer / fadeOutTime));
				int intValue = (int)floatValue;
				DWORD col = colorAdd(0xffffffff, -intValue);
				images[getCurrentImageIndex()]->SetColor(col);
			}
		}
	}
}

void GameOverScene::controlText(void)
{
	int ind = getCurrentImageIndex();
	
    if (ind == GAME_OVER_SCENE_TEXT_IMAGE_INDEX)
	{	
		//fading effects:

		//fade in:
		float fadeInTime = GAME_OVER_SCENE_IMAGE_FADE_TIME;
		if (imageTimer > GAME_OVER_SCENE_TEXT_TIME - fadeInTime)
		{
			float time = imageTimer - (GAME_OVER_SCENE_TEXT_TIME - fadeInTime);
			float floatValue = 255.0f * (time / fadeInTime);
			int intValue = (int)floatValue;
			DWORD col = colorAdd(0xffffffff, -intValue);
			textObject->SetColor(col);
		}

		//fade out:
		float fadeOutTime = GAME_OVER_SCENE_IMAGE_FADE_TIME;
		if (imageTimer < fadeOutTime)
		{
			float floatValue = 255.0f * (1.0f - (imageTimer / fadeOutTime));
			int intValue = (int)floatValue;
			DWORD col = colorAdd(0xffffffff, -intValue);
			textObject->SetColor(col);
		}
	}
}

void GameOverScene::renderImages(void)
{
	int ind = getCurrentImageIndex();
	
	if (currentImage)
	{
		currentImage->RenderEx(Game::SCREENWIDTH / 2, Game::SCREENHEIGHT / 2, 0, imageScales[getCurrentImageIndex()], imageScales[getCurrentImageIndex()]);
		images_showing = true;
	}
}

void GameOverScene::renderText(void)
{
	int ind = getCurrentImageIndex();
	
    if (ind == GAME_OVER_SCENE_TEXT_IMAGE_INDEX)
	{
		fnt->SetScale(2.0f);
		if (textObject)
		{
			textObject->Render();
		}
	}
}


#define LEVEL_FINISHED_SCENE_TIME 5.0f
#define LEVEL_FINISHED_SCENE_STAT_INCREMENT_TIME 0.05f
#define LEVEL_FINISHED_SCENE_STAT_INTERVAL 0.1f

#define LEVEL_FINISHED_SCENE_HEADER "LEVEL STATISTICS"
#define LEVEL_FINISHED_SCENE_INFANTRY_TEXT "Decapasitated enemy infantry: %i"
#define LEVEL_FINISHED_SCENE_TANKS_TEXT "Decapasitated enemy tanks: %i"

LevelFinishedScene::LevelFinishedScene(void)
{
	key_pressed = false;

	incrementSpeed = LEVEL_FINISHED_SCENE_STAT_INCREMENT_TIME;
	statInterval = LEVEL_FINISHED_SCENE_STAT_INTERVAL;
	sceneTimer = LEVEL_FINISHED_SCENE_TIME;
	statsIndex = 0;
	next_index = false;
	statTimer = 0.0f;
	p1StatTimer = 0.5f;
	p2StatTimer = 0.0f;
	
	nPlayer1EnemyInfantryTotalSum = 0;
	nPlayer2EnemyInfantryTotalSum = 0;
 	nPlayer1EnemyInfantryTotal = 0;
	nPlayer2EnemyInfantryTotal = 0;
	player1LevelScoreSum = 0;
	player2LevelScoreSum = 0;

	for (int i = 0; i < FootSoldier::NUMBER_OF_CLASSES; i++)
	{
		nPlayer1EnemyInfantryTotalSum += game.player_1.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][i];
		nPlayer2EnemyInfantryTotalSum += game.player_2.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][i];
        nPlayer1EnemyInfantry[i] = 0;
		nPlayer2EnemyInfantry[i] = 0;

		int sc1 = game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][i];
		player1LevelScoreSum += game.player_1.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][i] * sc1; 
		player2LevelScoreSum += game.player_2.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][i] * sc1;
	}

	nPlayer1EnemyInfantryTotal = 0;
	nPlayer2EnemyInfantryTotal = 0;
	nPlayer1EnemyTanks = 0;
	nPlayer2EnemyTanks = 0;
	player1LevelScore = 0;
	player2LevelScore = 0;
	player1LevelScoreSum += game.player_1.nDecapasitatedTanksLevel[CombatObject::ATTACKER_SIDE] * game.scores[CombatObject::ATTACKER_SIDE][GameObject::TANK][0];
	player2LevelScoreSum += game.player_2.nDecapasitatedTanksLevel[CombatObject::ATTACKER_SIDE] * game.scores[CombatObject::ATTACKER_SIDE][GameObject::TANK][0];
}

LevelFinishedScene::~LevelFinishedScene(void)
{
}

bool LevelFinishedScene::play(void)
{
	timerDelta = hge->Timer_GetDelta();
	sceneTimer -= timerDelta;

	int key = hge->Input_GetKey();
	if (key)
	{
		if ((key_pressed && key) ||
			sceneTimer < 0.0f)
		{
			return false;
		}
		else
		{
			key_pressed = true;
			incrementSpeed = 0;
			statInterval = 0;
		}
	}
	render();
	return true;
}


#define LEVEL_FINISHED_SCENE_HEIGHT_BETWEEN_STAT_ITEMS 30.0f
#define LEVEL_FINISHED_SCENE_HEIGHT_BETWEEN_STAT_GROUPS 50.0f
#define LEVEL_FINISHED_SCENE_STAT_INCREMENT_ACCELERATION_PER_UNIT 0.0005f

void LevelFinishedScene::render(void)
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);

	//render stats
	{

		fnt->SetScale(0.5f);
		fnt->SetColor(0xffffffff);

		char * header = LEVEL_FINISHED_SCENE_HEADER;

		float headerX = Game::SCREENWIDTH / 2.0f;
		float headerY = 30.0f;
		renderCenteredText(headerX, headerY, header, fnt);
		float player1StatsCenterX = Game::SCREENWIDTH / 4;
		float player2StatsCenterX = Game::SCREENWIDTH / 4 * 3;
		float player1StatX = 80.0f;
		float player2StatX = 480.0f;
		float playerStatsY = 100.0f;
		float sideStatsX = Game::SCREENWIDTH / 2.0f;
		float sideStatsY = 0.0f;
		statTimer -= timerDelta;
		p1StatTimer -= timerDelta;
		p2StatTimer -= timerDelta;
		float textIndent = 20.0f;

		//print player headers
		if (statsIndex == 0)
		{
			if (statTimer < 0.0f)
			{
				statsIndex++;
				statTimer = statInterval;
			}
		}
		if (statsIndex > 0)
		{
			char text[128];
			sprintf(text, "PLAYER 1");
			renderCenteredText(player1StatsCenterX, playerStatsY, text, fnt);
			sprintf(text, "PLAYER 2");
			renderCenteredText(player2StatsCenterX, playerStatsY, text, fnt);

			if (statsIndex == 1)
			{
				if (statTimer < 0.0f)
				{
					statsIndex++;
					statTimer = FLT_MAX;
					next_index = false;
				}
			}
		}

		//print total foot soldiers
		if (statsIndex > 1)
		{
			playerStatsY += LEVEL_FINISHED_SCENE_HEIGHT_BETWEEN_STAT_ITEMS;
			fnt->printf(player1StatX, playerStatsY, "Decapasitated enemy infantry   %i", nPlayer1EnemyInfantryTotal);
			fnt->printf(player2StatX, playerStatsY, "Decapasitated enemy infantry   %i", nPlayer2EnemyInfantryTotal);

			if (statsIndex == 2)
			{
				if (p1StatTimer < 0.0f && nPlayer1EnemyInfantryTotal < nPlayer1EnemyInfantryTotalSum)
				{
					//float & number = nPlayer2EnemyInfantryTotal;
					nPlayer1EnemyInfantryTotal++;
					p1StatTimer = incrementSpeed - nPlayer1EnemyInfantryTotal * LEVEL_FINISHED_SCENE_STAT_INCREMENT_ACCELERATION_PER_UNIT;
				}
				if (p2StatTimer < 0.0f && nPlayer2EnemyInfantryTotal < nPlayer2EnemyInfantryTotalSum)
				{
					nPlayer2EnemyInfantryTotal++;
					p2StatTimer = incrementSpeed - nPlayer2EnemyInfantryTotal * LEVEL_FINISHED_SCENE_STAT_INCREMENT_ACCELERATION_PER_UNIT;
				}
		
				if (nPlayer1EnemyInfantryTotal == nPlayer1EnemyInfantryTotalSum &&
					nPlayer2EnemyInfantryTotal == nPlayer2EnemyInfantryTotalSum &&
					!next_index)
				{
					statTimer = statInterval;
					next_index = true;
				}

				if (statTimer < 0.0f)
				{
					statsIndex++;
					statTimer = FLT_MAX;
					next_index = false;
				}
			}
		}

		//print foot soldier subcategories
		int fstypes = 5;
		char fsStrings[5][32] = {"Riflemen", "Machine gunners", "AT Soldiers", "Medics", "Fleeing soldiers"};
		FootSoldier::FOOT_SOLDIER_CLASS types[5] = {FootSoldier::RIFLEMAN, FootSoldier::MACHINE_GUNNER, FootSoldier::AT_SOLDIER, FootSoldier::MEDIC/*, FootSoldier::FLEEING_SOLDIER*/};
		for (int i = 0; i < fstypes; i++)
		{
			if (statsIndex > 2 + i)
			{
				playerStatsY += LEVEL_FINISHED_SCENE_HEIGHT_BETWEEN_STAT_ITEMS;

				int sc1 = game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][types[i]];
				int ntp1 = nPlayer1EnemyInfantry[types[i]];
				fnt->printf(player1StatX + textIndent, playerStatsY, "- %s (%i)   %i", fsStrings[i], game.scores[GameObject::FOOT_SOLDIER][types[i]], nPlayer1EnemyInfantry[types[i]]);
				fnt->printf(player2StatX + textIndent, playerStatsY, "- %s (%i)   %i", fsStrings[i], game.scores[GameObject::FOOT_SOLDIER][types[i]], nPlayer2EnemyInfantry[types[i]]);

				if (statsIndex == 3 + i)
				{
					nPlayer1EnemyInfantry[types[i]]++;
					p1StatTimer = incrementSpeed - nPlayer1EnemyInfantry[types[i]] * LEVEL_FINISHED_SCENE_STAT_INCREMENT_ACCELERATION_PER_UNIT;
					nPlayer2EnemyInfantry[types[i]]++;
					p2StatTimer = incrementSpeed - nPlayer2EnemyInfantry[types[i]] * LEVEL_FINISHED_SCENE_STAT_INCREMENT_ACCELERATION_PER_UNIT;
			
					if (nPlayer1EnemyInfantry[types[i]] == game.player_1.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][types[i]] &&
						nPlayer2EnemyInfantry[types[i]] == game.player_2.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][types[i]] &&
						!next_index)
					{
						statTimer = statInterval;
						next_index = true;
					}
					if (statTimer < 0.0f)
					{
						statsIndex++;
						statTimer = FLT_MAX;
						next_index = false;
					}
				}
			}
		}

		//print tanks
		if (statsIndex > 7)
		{
			playerStatsY += LEVEL_FINISHED_SCENE_HEIGHT_BETWEEN_STAT_ITEMS;
			fnt->printf(player1StatX, playerStatsY, "Decapasitated enemy tanks (%i)   %i", game.scores[GameObject::TANK][0], nPlayer1EnemyTanks);
			fnt->printf(player2StatX, playerStatsY, "Decapasitated enemy tanks (%i)   %i", game.scores[GameObject::TANK][0], nPlayer2EnemyTanks);

			if (statsIndex == 8)
			{
				if (p1StatTimer < 0.0f && nPlayer1EnemyTanks < game.player_1.nDecapasitatedTanksLevel[CombatObject::ATTACKER_SIDE])
				{
					nPlayer1EnemyTanks++;
					p1StatTimer = incrementSpeed - nPlayer1EnemyTanks * LEVEL_FINISHED_SCENE_STAT_INCREMENT_ACCELERATION_PER_UNIT;
				}
				if (p2StatTimer < 0.0f && nPlayer2EnemyTanks < game.player_2.nDecapasitatedTanksLevel[CombatObject::ATTACKER_SIDE])
				{
					nPlayer2EnemyTanks++;
					p2StatTimer = incrementSpeed - nPlayer2EnemyTanks * LEVEL_FINISHED_SCENE_STAT_INCREMENT_ACCELERATION_PER_UNIT;
				}
		
				if (nPlayer1EnemyTanks == game.player_1.nDecapasitatedTanksLevel[CombatObject::ATTACKER_SIDE] &&
					nPlayer2EnemyTanks == game.player_2.nDecapasitatedTanksLevel[CombatObject::ATTACKER_SIDE] &&
					!next_index)
				{
					statTimer = statInterval;
					next_index = true;
				}

				if (statTimer < 0.0f)
				{
					statsIndex++;
					statTimer = FLT_MAX;
					next_index = false;
				}
			}
		}

		//print level scores
		if (statsIndex > 8)
		{
			playerStatsY += LEVEL_FINISHED_SCENE_HEIGHT_BETWEEN_STAT_ITEMS;
			fnt->printf(player1StatX, playerStatsY, "Level score   %i", player1LevelScore);
			fnt->printf(player2StatX, playerStatsY, "Level score   %i", player2LevelScore);

			if (statsIndex == 9)
			{
				if (p1StatTimer < 0.0f && player1LevelScore < player1LevelScoreSum)
				{
					player1LevelScore++;
					p1StatTimer = incrementSpeed - player1LevelScore * LEVEL_FINISHED_SCENE_STAT_INCREMENT_ACCELERATION_PER_UNIT;
				}
				if (p2StatTimer < 0.0f && player2LevelScore < player2LevelScoreSum)
				{
					player2LevelScore++;
					p2StatTimer = incrementSpeed - player2LevelScore * LEVEL_FINISHED_SCENE_STAT_INCREMENT_ACCELERATION_PER_UNIT;
				}
		
				if (player1LevelScore == player1LevelScoreSum &&
					player2LevelScore == player2LevelScoreSum &&
					!next_index)
				{
					statTimer = statInterval;
					next_index = true;
				}

				if (statTimer < 0.0f)
				{
					statsIndex++;
					statTimer = statInterval;
					next_index = false;
				}
			}
		}

		if (statsIndex > 9)
		{
			playerStatsY += LEVEL_FINISHED_SCENE_HEIGHT_BETWEEN_STAT_ITEMS;
			fnt->printf(player1StatX, playerStatsY, "Total score   %i", game.player_1.score);
			fnt->printf(player2StatX, playerStatsY, "Total score   %i", game.player_2.score);

			if (statsIndex == 10)
			{
				if (statTimer < 0.0f)
				{
					statsIndex++;
					statTimer = statInterval;
					next_index = false;
				}
			}
		}
	}
	hge->Gfx_EndScene();
}


#define MESSAGE_CENTER_X 400.0f
#define MESSAGE_SCALE 0.7f

Message::Message(void)
{
	nextMessage = NULL;
	sprintf(text, "");
	messageDelayTimer = -1.0f;
	addTime = 0.0f;
	color = 0xffffffff;
	y = 0.0f;
}

Message::Message(const char * text, DWORD color, Time delay)
{
	this->nextMessage = NULL;
	sprintf(this->text, text);
	this->messageDelayTimer = delay;
	this->addTime = 0.0f;
	this->color = color;
	this->y = 0.0f;
}

void Message::render(void)
{
	fnt->SetScale(MESSAGE_SCALE);
	fnt->SetColor(color);
	renderCenteredText(MESSAGE_CENTER_X, y, text, fnt);
}

void Message::setY(float y)
{
	this->y = y;
}

float Message::getY()
{
	return y;
}


#define FIRST_MESSAGE_Y 300
#define MESSAGE_Y_SPACING 40.0f
#define MESSAGE_DISPLAY_TIME 5.0f

MessageManager::MessageManager(void)
{
	waitingMessagesNod = new Message();
	for (int i = 0; i < MESSAGES_MAX; i++)
		messages[i] = NULL;

	nextMessageY = FIRST_MESSAGE_Y;
}

MessageManager::~MessageManager(void)
{
	clear();
	delete waitingMessagesNod;
	waitingMessagesNod = NULL;
}

void MessageManager::addMessage(const char * message, DWORD color, Time delay)
{
	Message * m = new Message(message, color, delay);
	if (delay > 0.0f)
	{
		Message * msg = waitingMessagesNod;
		while (msg->nextMessage != NULL)
		{
			msg = msg->nextMessage;
		}
		msg->nextMessage = m;
	}
	else
	{
		addMessage(m);
	}
}

void MessageManager::addMessage(Message * message)
{
	for (int i = 0; i < MESSAGES_MAX; i++)
	{
		if (messages[i] == NULL)
		{
			message->setY(nextMessageY);
			message->addTime = game.currentLevel->levelTime;
			messages[i] = message;
			nextMessageY += MESSAGE_Y_SPACING;
			return;
		}
	}

	//no space? - delete old messages

	delete messages[0];
	messages[0] = NULL;

	for (int i = 0; i < MESSAGES_MAX - 1; i++)
	{
		if (messages[i + 1] != NULL)
		{
			messages[i] = messages[i + 1];
			messages[i]->setY(messages[i]->getY() - MESSAGE_Y_SPACING);
			messages[i + 1] = NULL;
		}
		else
			break;
	}

	message->setY(nextMessageY);
	message->addTime = game.currentLevel->levelTime;
	messages[MESSAGES_MAX - 1] = message;
}

void MessageManager::render(void)
{
	//render
	{
		for (int i = 0; i < MESSAGES_MAX; i++)
		{
			if (messages[i] != NULL)
			{
				messages[i]->render();
			}
			else
				break;
		}
	}

	//update
	{
		if (messages[0])
		{
			if (game.currentLevel->levelTime - messages[0]->addTime > MESSAGE_DISPLAY_TIME)
			{
				delete messages[0];
				messages[0] = NULL;

				for (int i = 0; i < MESSAGES_MAX - 1; i++)
				{
					if (messages[i + 1] != NULL)
					{
						messages[i] = messages[i + 1];
						messages[i]->setY(messages[i]->getY() - MESSAGE_Y_SPACING);
						messages[i + 1] = NULL;
					}
					else
						break;
				}
				nextMessageY -= MESSAGE_Y_SPACING;
			}
		}
	}

	//handle awaiting messages
	{
		Message * msg = waitingMessagesNod;
		while (msg->nextMessage != NULL)
		{
			msg->nextMessage->messageDelayTimer -= timerDelta;
			if (msg->nextMessage->messageDelayTimer < 0.0f)
			{
				addMessage(msg->nextMessage);
				msg->nextMessage = msg->nextMessage->nextMessage;
			}
			else msg = msg->nextMessage;

			if (msg == NULL)
				break;
		}
	}
}

void MessageManager::init(void)
{
	nextMessageY = FIRST_MESSAGE_Y;
}

void MessageManager::clear(void)
{
	for (int i = 0; i < MESSAGES_MAX; i++)
	{
		if (messages[i] != NULL)
		{
			delete messages[i];
			messages[i] = NULL;
		}
	}

	Message * msg = waitingMessagesNod->nextMessage;
	Message * killer;
	while (msg != NULL)
	{
		killer = msg;
		msg = msg->nextMessage;
		delete killer;
	}
	waitingMessagesNod->nextMessage = NULL;
}


int Game::SCREENWIDTH = 800;
int Game::SCREENHEIGHT = 600;
int Game::MIDSCREENWIDTH = SCREENWIDTH / 2;
int Game::MIDSCREENHEIGHT = SCREENHEIGHT / 2;
float Game::FIRE_START_Y = 50.0f;
Distance Game::METER = 8.97250f; //Meter in pixels. Which is 62 pixels (tank length) / 6.91 meters (t-82 length). Which makes the soldiers a bit tall.
Distance Game::KILOMETER = METER * 1000.0f;
Distance Game::BIG_DISTANCE = 999999.0f;
Distance Game::GROUND_DISTANCE_FROM_SCREEN = 1 / (METER * 0.2f / 600.0f);
Time Game::MESSAGE_TIME = 3.0f;
#define INFO_TEXT_SIZE 0.7f
#define INFO_TEXT_TIME 3.0f

#define QUIT_GAME_TIME 5.0f
#define GAME_OVER_DELAY 3.0f
#define FADING_TIME 0.5f
#define STATE_CHANGE_TIME_LEVEL_FINISHED 5.0f //DEBUG!!5.0f
#define STATE_CHANGE_TIME_GAME_OVER 3.0f
#define STATE_CHANGE_TIME_GAME_FINISHED 3.0f
#define MAX_BLOOD 300
#define TRAINING_LEVEL_MESSAGE_TIME 7.0f
#define TRAINING_LEVEL_TIME 14 * TRAINING_LEVEL_MESSAGE_TIME

Game::Game(void)
{
	previousState = (STATE)-1;
	state = (STATE)0;
	nextState = (STATE)-1;
	setState(GAME_LOAD);

	two_player_game = false;
	application_close_flag = false;
	timeAdjust = 1.0f;
	zoomLevel = 1.0f;

	dpx = 0.0f;
	dpy = 0.0f;

	stateTimer = 0.0f;
	fadeOutTimer = 0.0f;
	fadeInTimer = 0.0f;

	outGameScene = NULL;
	take_input = true;
	training = false;

	startlevelNumber = 0;
}

Game::~Game(void)
{
}

//State
bool Game::run(void)
{
	int key = hge->Input_GetKey();
	if (key == HGEK_B)
	{
		int u = 2;		
	}

	setTimerDelta();
	processStateFlow();

	switch (state)
	{
	case GAME_LOAD: loadGame(); break;
	case INTRO: playIntro(); break;
	case MENU: menu(); break;
	case OPTIONS_MENU: optionsDialog(); break;
	case START_TRAINING_GAME: startTrainingGame(); break;
	case START_GAME: startGame(); break;
	case LEVEL_LOAD: loadLevel(); break;
	case LEVEL_START_INTRO: levelStartIntro(); break;
	case LEVEL_RUN: runLevel(); break;
	case PAUSED: pause(); break;
	case MIDLEVEL_DIALOG: midLevelDialog(); break;
	case LEVEL_FINISHED: levelFinished(); break;
	case GAME_FINISHED: gameFinished(); break;
	case GAME_OVER: gameOver(); break;
	case QUIT_GAME: quit(); break;
	default: break;
	}

	return application_close_flag;
}

void Game::loadGame(void)
{
	//text objects:
	{
		int ind = 0;
		textObjects[TEXT_OBJECT_MIDSCREEN] = new hgeGUIText(ind++, 200.0f, 200.0f, 400.0f, 200.0f, fnt);
		textObjects[TEXT_OBJECT_LEVEL_INTRO_HEADER] = new hgeGUIText(ind++, 200.0f, 0.0f, 400.0f, 200.0f, fnt);
		textObjects[TEXT_OBJECT_LEVEL_INTRO_HEADER_SHADOW] = new hgeGUIText(ind++, 180.0f, 10.0f, 400.0f, 200.0f, fnt);
		textObjects[TEXT_OBJECT_QUIT_GAME_TEXT_BOX] = new hgeGUIText(ind++, 200.0f, 100.0f, 400.0f, 200.0f, fnt);
		textObjects[TEXT_OBJECT_QUIT_GAME_TEXT_BOX_SHADOW] = new hgeGUIText(ind++, 190.0f, 110.0f, 400.0f, 200.0f, fnt);

		for (int i = 0; i < TEXT_OBJECT_MAX; i++)
		{
			textObjects[i]->SetMode(HGETEXT_CENTER);
		}
	}

	renderLoadScreen("INITIALIZING");

	//WORK:

	hge->Random_Seed();

	//settings
	{
		hge->System_SetState(HGE_INIFILE, "Settings.ini");

		//Options:
		{
			show_collisionspheres = (hge->Ini_GetInt("OPTIONS", "SHOW_COLLISIONSPHERES", 0) == 1);
			show_locations = (hge->Ini_GetInt("OPTIONS", "SHOW_LOCATIONS", 0) == 1);
			show_mg_tracers = (hge->Ini_GetInt("OPTIONS", "SHOW_MG_TRACERS", 0) == 1);
			show_cannon_tracers = (hge->Ini_GetInt("OPTIONS", "SHOW_CANNON_TRACERS", 0) == 1);
			show_debug_text = (hge->Ini_GetInt("OPTIONS", "SHOW_DEBUG_TEXT", 0) == 1);
			list_all_objects = (hge->Ini_GetInt("OPTIONS", "LIST_ALL_OBJECTS", 0) == 1);
			friendly_fire = (hge->Ini_GetInt("OPTIONS", "FRIENDLY_FIRE", 0) == 1);
			time_adjust = (hge->Ini_GetInt("OPTIONS", "TIME_ADJUST", 0) == 1);
			zoom = (hge->Ini_GetInt("OPTIONS", "ZOOM", 0) == 1);
			play_sounds = (hge->Ini_GetInt("OPTIONS", "PLAY_SOUNDS", 1) == 1);
			display_messages = (hge->Ini_GetInt("OPTIONS", "DISPLAY_MESSAGES", 1) == 1);
			modify_background = (hge->Ini_GetInt("OPTIONS", "MODIFY_BACKGROUND", 1) == 1);
			musicVolume = 100;
			fxVolume = 100;
			amountOfParticles = hge->Ini_GetInt("OPTIONS", "AMOUNT_OF_PARTICLES", 100);
			amountOfBlood = hge->Ini_GetInt("OPTIONS", "AMOUNT_OF_BLOOD", 100);
			maxShadows = hge->Ini_GetInt("OPTIONS", "MAX_NUMBER_OF_SHADOWS", 100);
			godMode = hge->Ini_GetInt("OPTIONS", "GOD_MODE", 0);
		}

		//skill level
		{
			skillLevelNames[SKILL_LEVEL_EASY] = "EASY";
			skillLevelNames[SKILL_LEVEL_NORMAL] = "NORMAL";
			skillLevelNames[SKILL_LEVEL_HARD] = "HARD";
			skillLevel = SKILL_LEVEL_NORMAL;
		}

		//scores
		{	
			for (int i = 0; i < CombatObject::NUMBER_OF_SIDES; i++)
			{
				for (int j = 0; j < GameObject::NUMBER_OF_TYPES; j++)
				{
					for (int k = 0; k < GameObject::SUBTYPE_MAX; k++)
					{
						scores[i][j][k] = 0;
					}
				}
			}

			scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::RIFLEMAN] = hge->Ini_GetInt("SCORES", "ENEMY_RIFLEMAN", 0);
			scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::MACHINE_GUNNER] = hge->Ini_GetInt("SCORES", "ENEMY_MACHINE_GUNNER", 0);
			scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::AT_SOLDIER] = hge->Ini_GetInt("SCORES", "ENEMY_AT_SOLDIER", 0);
			scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::MEDIC] = hge->Ini_GetInt("SCORES", "ENEMY_MEDIC", 0);
			scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::CAPTIVE_KILLED] = hge->Ini_GetInt("SCORES", "ENEMY_CAPTIVE_KILLED", 0);
			scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::CAPTIVE_TAKEN] = hge->Ini_GetInt("SCORES", "ENEMY_CAPTIVE_TAKEN", 0);
			scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::FLEEING_SOLDIER] = hge->Ini_GetInt("SCORES", "ENEMY_FLEEING_SOLDIER", 0);
			scores[CombatObject::ATTACKER_SIDE][GameObject::TANK][0] = hge->Ini_GetInt("SCORES", "ENEMY_TANK", 0);

			scores[CombatObject::DEFENDER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::RIFLEMAN] = hge->Ini_GetInt("SCORES", "OWN_SIDE_RIFLEMAN", 0);
			scores[CombatObject::DEFENDER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::MACHINE_GUNNER] = hge->Ini_GetInt("SCORES", "OWN_SIDE_MACHINE_GUNNER", 0);
			scores[CombatObject::DEFENDER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::AT_SOLDIER] = hge->Ini_GetInt("SCORES", "OWN_SIDE_AT_SOLDIER", 0);
			scores[CombatObject::DEFENDER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::MEDIC] = hge->Ini_GetInt("SCORES", "OWN_SIDE_MEDIC", 0);
			scores[CombatObject::DEFENDER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::CAPTIVE_KILLED] = hge->Ini_GetInt("SCORES", "OWN_SIDE_CAPTIVE_KILLED", 0);
			scores[CombatObject::DEFENDER_SIDE][GameObject::FOOT_SOLDIER][Player::INFANTRY_SCORES::FLEEING_SOLDIER] = hge->Ini_GetInt("SCORES", "OWN_SIDE_FLEEING_SOLDIER", 0);
			scores[CombatObject::DEFENDER_SIDE][GameObject::TANK][0] = hge->Ini_GetInt("SCORES", "OWN_SIDE_TANK", 0);
		}
	}

	//textures:
	{
		//texture for particles
		particletex = hge->Texture_Load("particles.png");
		particletex2 = hge->Texture_Load("particles2.png");
		particletex3 = hge->Texture_Load("particles3.png");

		//textures:
		if ((textures[TEXTURE_LIGHT] = hge->Texture_Load("light.png"))  == 0) MessageBox(NULL, "Could not load texture: light.png", "info", MB_OK);
		if ((textures[TEXTURE_DEBRIS] = hge->Texture_Load("debris.png"))  == 0) MessageBox(NULL, "Could not load texture: effects.png", "info", MB_OK);
		if ((textures[TEXTURE_EFFECTS] = hge->Texture_Load("effects.png"))  == 0) MessageBox(NULL, "Could not load texture: effects.png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_ATTACKER_RIFLEMAN] = hge->Texture_Load("rifleman.png"))  == 0) MessageBox(NULL, "Could not load texture: rifleman.png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_ATTACKER_MACHINE_GUNNER] = hge->Texture_Load("machine_gunner.png"))  == 0) MessageBox(NULL, "Could not load texture: machine_gunner.png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_ATTACKER_AT_SOLDIER] = hge->Texture_Load("at_soldier.png"))  == 0) MessageBox(NULL, "Could not load texture: at_soldier.png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_ATTACKER_SNIPER] = hge->Texture_Load("rifleman.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_ATTACKER_MEDIC] = hge->Texture_Load("medic.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_DEFENDER_RIFLEMAN] = hge->Texture_Load("rifleman_green.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_DEFENDER_MACHINE_GUNNER] = hge->Texture_Load("machine_gunner_green.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_DEFENDER_AT_SOLDIER] = hge->Texture_Load("at_soldier_green.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_DEFENDER_SNIPER] = hge->Texture_Load("rifleman_green.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_DEFENDER_MEDIC] = hge->Texture_Load("medic_green.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_FOOT_SOLDIER_BODY_PARTS] = hge->Texture_Load("body_parts.png"))  == 0) MessageBox(NULL, "Could not load texture: body_parts.png", "info", MB_OK);
		if ((textures[TEXTURE_BUSH_1] = hge->Texture_Load("bush1.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_BUSH_2] = hge->Texture_Load("bush2.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_BUSH_3] = hge->Texture_Load("bush3.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_BUSH_4] = hge->Texture_Load("bush4.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_1] = hge->Texture_Load("intro_1.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_2] = hge->Texture_Load("intro_2.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_3] = hge->Texture_Load("intro_3.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_4] = hge->Texture_Load("intro_4.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_5] = hge->Texture_Load("intro_5.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_6] = hge->Texture_Load("intro_6.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_7] = hge->Texture_Load("intro_7.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_8] = hge->Texture_Load("intro_8.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_9] = hge->Texture_Load("intro_9.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_10] = hge->Texture_Load("intro_10.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_11] = hge->Texture_Load("intro_11.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_12] = hge->Texture_Load("intro_12.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_13] = hge->Texture_Load("intro_13.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_14] = hge->Texture_Load("intro_14.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_INTRO_IMAGE_15] = hge->Texture_Load("intro_15.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);		
		if ((textures[TEXTURE_TITLE] = hge->Texture_Load("title.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_EMPTY] = hge->Texture_Load("empty.png"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		textures[TEXTURE_FADE] = hge->Texture_Create(Game::SCREENWIDTH, Game::SCREENHEIGHT);
		if ((textures[TEXTURE_GUI_BACKGROUND_1] = hge->Texture_Load("camo1.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_GAME_OVER_1] = hge->Texture_Load("gameover_1.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_GAME_OVER_2] = hge->Texture_Load("gameover_2.jpg"))  == 0) MessageBox(NULL, "Could not load texture: .png", "info", MB_OK);
		if ((textures[TEXTURE_VICTORY_1] = hge->Texture_Load("outro_1.jpg"))  == 0) MessageBox(NULL, "Could not load texture: 'outro1.jpg'", "info", MB_OK);
		if ((textures[TEXTURE_VICTORY_2] = hge->Texture_Load("outro_2.jpg"))  == 0) MessageBox(NULL, "Could not load texture: 'outro2.jpg'", "info", MB_OK);
		if ((textures[TEXTURE_VICTORY_3] = hge->Texture_Load("outro_3.jpg"))  == 0) MessageBox(NULL, "Could not load texture: 'outro3.jpg'", "info", MB_OK);
		if ((textures[TEXTURE_VICTORY_4] = hge->Texture_Load("outro_4.jpg"))  == 0) MessageBox(NULL, "Could not load texture: 'outro4.jpg'", "info", MB_OK);

		//texture modifications
		int w = hge->Texture_GetWidth(textures[TEXTURE_FADE]);
		DWORD * colArray = hge->Texture_Lock(textures[TEXTURE_FADE]);

		for (int i = 0; i < Game::SCREENHEIGHT; i++)
		{
			for (int j = 0; j < Game::SCREENWIDTH; j++)
			{
				//set black
				colArray[i * w + j] = 0xff000000;
			}
		}
		hge->Texture_Unlock(textures[TEXTURE_FADE]);
	}
	//animations:
	{
		for (int i = 0; i < ANIMATION_MAX; i++)
			animations[i] = NULL;
		
		animations[ANIMATION_SHADOW] = new hgeAnimation(textures[TEXTURE_EFFECTS], 1, 0.0f, 0, 400, 48, 32); animations[ANIMATION_SHADOW]->SetHotSpot(24, 16); animations[ANIMATION_SHADOW]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE); animations[ANIMATION_SHADOW]->SetColor(0, 1); animations[ANIMATION_SHADOW]->SetColor(0, 2);
		animations[ANIMATION_LIGHT] = new hgeAnimation(textures[TEXTURE_LIGHT], 1, 0.0f, 0, 0, 128, 128); animations[ANIMATION_LIGHT]->SetHotSpot(64, 64); animations[ANIMATION_LIGHT]->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_NOZWRITE); animations[ANIMATION_LIGHT]->SetColor(getColor(255, 200, 200, 150));
		animations[ANIMATION_WATER_DROP] = new hgeAnimation(particletex3, 1, 0.0f, 3 * 32.0f, 3 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_WATER_DROP]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_WATER_DROP]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_DEBRIS_1] = new hgeAnimation(textures[TEXTURE_DEBRIS], 24, 10.0f, 0 * 32.0f, 0 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_DEBRIS_1]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_DEBRIS_1]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_DEBRIS_MAX] = new hgeAnimation(textures[TEXTURE_DEBRIS], 24, 10.0f, 0 * 32.0f, 1 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_DEBRIS_MAX]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_DEBRIS_MAX]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_CAMO_NET_1] = new hgeAnimation(textures[TEXTURE_DEBRIS], 1, 0.0f, 0 * 32.0f, 3 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_CAMO_NET_1]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_CAMO_NET_1]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_SANDBAG] = new hgeAnimation(textures[TEXTURE_DEBRIS], 1, 0.0f, 0 * 32.0f, 4 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_SANDBAG]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_SANDBAG]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_CLOUD_1] = new hgeAnimation(particletex, 1, 0.0f, 2 * 32.0f, 0 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_CLOUD_1]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_CLOUD_1]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_CLOUD_2] = new hgeAnimation(particletex, 1, 0.0f, 3 * 32.0f, 3 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_CLOUD_2]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_CLOUD_2]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_BOLT] = new hgeAnimation(particletex2, 1, 0.0f, 2 * 32.0f, 2 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_BOLT]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_BOLT]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_FLAME] = new hgeAnimation(particletex2, 1, 0.0f, 1 * 32.0f, 2 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_FLAME]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_FLAME]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_RUBBLE] = new hgeAnimation(particletex2, 1, 0.0f, 0 * 32.0f, 1 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_RUBBLE]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_RUBBLE]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_FIRE_CLOUD] = new hgeAnimation(particletex, 1, 0.0f, 0 * 32.0f, 3 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_FIRE_CLOUD]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_FIRE_CLOUD]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_SMOKE_1_LIGHT] = new hgeAnimation(particletex, 1, 0.0f, 3 * 32.0f, 1 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_1_LIGHT]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_1_LIGHT]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_SMOKE_2_LIGHT] = new hgeAnimation(particletex, 1, 0.0f, 0 * 32.0f, 2 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_2_LIGHT]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_2_LIGHT]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_SMOKE_3_LIGHT] = new hgeAnimation(particletex, 1, 0.0f, 2 * 32.0f, 2 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_3_LIGHT]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_3_LIGHT]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_SMOKE_4_LIGHT] = new hgeAnimation(particletex, 1, 0.0f, 0 * 32.0f, 3 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_4_LIGHT]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_4_LIGHT]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_SMOKE_1_DARK] = new hgeAnimation(particletex2, 1, 0.0f, 3 * 32.0f, 1 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_1_DARK]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_1_DARK]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_SMOKE_2_DARK] = new hgeAnimation(particletex2, 1, 0.0f, 0 * 32.0f, 2 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_2_DARK]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_2_DARK]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_SMOKE_3_DARK] = new hgeAnimation(particletex2, 1, 0.0f, 2 * 32.0f, 2 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_3_DARK]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_3_DARK]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_PARTICLE_SPRITE_SMOKE_4_DARK] = new hgeAnimation(particletex2, 1, 0.0f, 0 * 32.0f, 3 * 32.0f, 32.0f, 32.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_4_DARK]->SetHotSpot(16.0f, 16.0f); animations[ANIMATION_PARTICLE_SPRITE_SMOKE_4_DARK]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_BUSH_1] = new hgeAnimation(textures[TEXTURE_BUSH_1], 1, 0.0f, 0.0f, 0.0f, 64.0f, 64.0f); animations[ANIMATION_BUSH_1]->SetHotSpot(32.0f, 32.0f);
		animations[ANIMATION_BUSH_2] = new hgeAnimation(textures[TEXTURE_BUSH_2], 1, 0.0f, 0.0f, 0.0f, 64.0f, 64.0f); animations[ANIMATION_BUSH_2]->SetHotSpot(32.0f, 32.0f);
		animations[ANIMATION_BUSH_3] = new hgeAnimation(textures[TEXTURE_BUSH_3], 1, 0.0f, 0.0f, 0.0f, 64.0f, 64.0f); animations[ANIMATION_BUSH_3]->SetHotSpot(32.0f, 32.0f);
		animations[ANIMATION_BUSH_4] = new hgeAnimation(textures[TEXTURE_BUSH_4], 1, 0.0f, 0.0f, 0.0f, 64.0f, 64.0f); animations[ANIMATION_BUSH_4]->SetHotSpot(32.0f, 32.0f);
		animations[ANIMATION_ASSAULT_RIFLE_MUZZLE_FLASH] = new hgeAnimation(/*texture*/textures[TEXTURE_EFFECTS], /*frames*/1, /*fps*/0.0f, /*x*/0.0f, /*y*/222.0f, /*width*/16.0f, /*height*/16.0f); animations[ANIMATION_ASSAULT_RIFLE_MUZZLE_FLASH]->SetHotSpot(8.0f, 8.0f);
		animations[ANIMATION_TANK_CANNON_MUZZLE_FLASH] = new hgeAnimation(/*texture*/textures[TEXTURE_EFFECTS], /*frames*/1, /*fps*/0.0f, /*x*/0.0f, /*y*/257.0f, /*width*/64.0f, /*height*/64.0f); animations[ANIMATION_TANK_CANNON_MUZZLE_FLASH]->SetHotSpot(32.0f, 32.0f); //animations[ANIMATION_TANK_CANNON_MUZZLE_FLASH]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_HAND_GRENADE_EXPLOSION_CRATER] = new hgeAnimation(/*texture*/textures[TEXTURE_EFFECTS], /*frames*/1, /*fps*/0.0f, /*x*/152.0f, /*y*/176.0f, /*width*/32.0f, /*height*/32.0f); animations[ANIMATION_HAND_GRENADE_EXPLOSION_CRATER]->SetHotSpot(16.0f, 16.0f); //animations[ANIMATION_TANK_CANNON_MUZZLE_FLASH]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_INTRO_IMAGE_1] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_1], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 800, 533); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_1]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_1]));
		animations[ANIMATION_INTRO_IMAGE_2] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_2], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 800, 522); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_2]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_2]));
		animations[ANIMATION_INTRO_IMAGE_3] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_3], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 800, 557); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_3]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_3]));
		animations[ANIMATION_INTRO_IMAGE_4] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_4], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 916, 1084); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_4]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_4]));
		animations[ANIMATION_INTRO_IMAGE_5] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_5], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 800, 575); // /*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_5]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_5]));
		animations[ANIMATION_INTRO_IMAGE_6] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_6], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 640, 457); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_6]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_6]));
		animations[ANIMATION_INTRO_IMAGE_7] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_7], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 510, 369); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_7]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_7]));
		animations[ANIMATION_INTRO_IMAGE_8] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_8], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 510, 308); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_8]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_8]));
		animations[ANIMATION_INTRO_IMAGE_9] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_9], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 420, 220); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_9]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_9]));
		animations[ANIMATION_INTRO_IMAGE_10] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_10], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 640, 480); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_10]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_10]));
		animations[ANIMATION_INTRO_IMAGE_11] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_11], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 320, 227); //*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_11]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_11]));
		animations[ANIMATION_INTRO_IMAGE_12] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_12], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 510, 317); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_12]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_12]));
		animations[ANIMATION_INTRO_IMAGE_13] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_13], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 800, 554); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_13]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_13]));
		animations[ANIMATION_INTRO_IMAGE_14] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_14], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 315, 231); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_14]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_14]));
		animations[ANIMATION_INTRO_IMAGE_15] = new hgeAnimation(/*texture*/textures[TEXTURE_INTRO_IMAGE_15], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 357, 240); ///*width*/hge->Texture_GetWidth(textures[TEXTURE_INTRO_IMAGE_15]), /*height*/hge->Texture_GetHeight(textures[TEXTURE_INTRO_IMAGE_15]));
		animations[ANIMATION_TITLE] = new hgeAnimation(/*texture*/textures[TEXTURE_TITLE], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, /*width*/400, /*height*/100); animations[ANIMATION_TITLE]->SetHotSpot(200, 50); //animations[ANIMATION_TANK_CANNON_MUZZLE_FLASH]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
		animations[ANIMATION_TITLE_GLOOM] = new hgeAnimation(/*texture*/textures[TEXTURE_TITLE], /*frames*/1, /*fps*/0.0f, /*x*/400, /*y*/0, /*width*/100, /*height*/100); animations[ANIMATION_TITLE_GLOOM]->SetHotSpot(50, 50); animations[ANIMATION_TITLE_GLOOM]->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
		animations[ANIMATION_FADE_FILTER] = new hgeAnimation(/*texture*/textures[TEXTURE_FADE], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, /*width*/Game::SCREENWIDTH, /*height*/Game::SCREENHEIGHT); animations[ANIMATION_FADE_FILTER]->SetHotSpot(SCREENWIDTH / 2, SCREENHEIGHT / 2);
		animations[ANIMATION_GUI_BACKGROUND_1] = new hgeAnimation(/*texture*/textures[TEXTURE_GUI_BACKGROUND_1], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, /*width*/1024, /*height*/1024); animations[ANIMATION_GUI_BACKGROUND_1]->SetHotSpot(512, 512);
		animations[ANIMATION_GAME_OVER_1] = new hgeAnimation(/*texture*/textures[TEXTURE_GAME_OVER_1], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 640, 280); animations[ANIMATION_GAME_OVER_1]->SetHotSpot(640 / 2, 280 / 2);
		animations[ANIMATION_GAME_OVER_2] = new hgeAnimation(/*texture*/textures[TEXTURE_GAME_OVER_2], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 400, 300); animations[ANIMATION_GAME_OVER_2]->SetHotSpot(400 / 2, 300 / 2);
		animations[ANIMATION_VICTORY_1] = new hgeAnimation(/*texture*/textures[TEXTURE_VICTORY_1], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 165, 217); animations[ANIMATION_VICTORY_1]->SetHotSpot(165 / 2, 217 / 2);
		animations[ANIMATION_VICTORY_2] = new hgeAnimation(/*texture*/textures[TEXTURE_VICTORY_2], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 276, 415); animations[ANIMATION_VICTORY_2]->SetHotSpot(276 / 2, 415 / 2);
		animations[ANIMATION_VICTORY_3] = new hgeAnimation(/*texture*/textures[TEXTURE_VICTORY_3], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 300, 180); animations[ANIMATION_VICTORY_3]->SetHotSpot(300 / 2, 180 / 2);
		animations[ANIMATION_VICTORY_4] = new hgeAnimation(/*texture*/textures[TEXTURE_VICTORY_4], /*frames*/1, /*fps*/0.0f, /*x*/0, /*y*/0, 550, 392); animations[ANIMATION_VICTORY_4]->SetHotSpot(550 / 2, 392 / 2);
	}

	//targets:
	{
		targets[TARGET_PARTICLES] = hge->Target_Create(SCREENWIDTH, SCREENHEIGHT, false);
	}

	//sounds:
	{
		for (int i = 0; i < SOUND_MAX; i++)
			sounds[i] = NULL;

		sounds[SOUND_SMOKE_GRENADE_SHOOT] = hge->Effect_Load("smoke_grenade_shoot 438148_SOUNDDOGS_GR.mp3");
		sounds[SOUND_SMOKE_GRENADE_EXPLODE] = hge->Effect_Load("smoke_grenade_explode 280858_SOUNDDOGS_ST.mp3");
		sounds[SOUND_FLARE] = hge->Effect_Load("flare.wav");
		sounds[SOUND_RAIN] = hge->Effect_Load("ambience_rain_1 e_rain_grass1.wav");
		sounds[SOUND_DEBRIS_HITS_GROUND_1] = hge->Effect_Load("442255_SOUNDDOGS_Hi debris_hit_ground_1.mp3");
		sounds[SOUND_DEBRIS_HITS_GROUND_2] = hge->Effect_Load("442256_SOUNDDOGS_Hi debris_hit_ground_2.mp3");
		sounds[SOUND_DEBRIS_HITS_GROUND_MAX] = hge->Effect_Load("442254_SOUNDDOGS_Hi debris_hit_ground_3.mp3");
		sounds[SOUND_DEBRIS_ROCK_HITS_GROUND_1] = hge->Effect_Load("rock_thud_1.mp3");
		sounds[SOUND_DEBRIS_ROCK_HITS_GROUND_2] = hge->Effect_Load("rock_thud_2.mp3");
		sounds[SOUND_DEBRIS_ROCK_HITS_GROUND_MAX] = hge->Effect_Load("rock_thud_3.mp3");
		sounds[SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_1] = hge->Effect_Load("489909_SOUNDDOGS_Sa sand_1.mp3");
		sounds[SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_2] = hge->Effect_Load("489909_SOUNDDOGS_Sa sand_2.mp3");
		sounds[SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_3] = hge->Effect_Load("489909_SOUNDDOGS_Sa sand_3.mp3");
		sounds[SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_4] = hge->Effect_Load("489909_SOUNDDOGS_Sa sand_4.mp3");
		sounds[SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_5] = hge->Effect_Load("489909_SOUNDDOGS_Sa sand_5.mp3");
		sounds[SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_MAX] = hge->Effect_Load("489909_SOUNDDOGS_Sa sand_6.mp3");
		sounds[SOUND_DEBRIS_HITS_METAL_1] = hge->Effect_Load("147299_SOUNDDOGS_ro debris_hit_metal_1.mp3");
		sounds[SOUND_DEBRIS_HITS_METAL_2] = hge->Effect_Load("147299_SOUNDDOGS_ro debris_hit_metal_2.mp3");
		sounds[SOUND_DEBRIS_HITS_METAL_MAX] = hge->Effect_Load("147299_SOUNDDOGS_ro debris_hit_metal_3.mp3");
		sounds[SOUND_DEBRIS_SMALL_HITS_METAL_1] = hge->Effect_Load("147299_SOUNDDOGS_ro small_debris_hit_metal_1.mp3");
		sounds[SOUND_DEBRIS_SMALL_HITS_METAL_2] = hge->Effect_Load("147299_SOUNDDOGS_ro small_debris_hit_metal_2.mp3");
		sounds[SOUND_DEBRIS_SMALL_HITS_METAL_MAX] = hge->Effect_Load("147299_SOUNDDOGS_ro small_debris_hit_metal_3.mp3");
		sounds[SOUND_DEBRIS_METAL_HITS_GROUND_1] = hge->Effect_Load("506279_SOUNDDOGS_Hi metal_hit_1.mp3");
		sounds[SOUND_DEBRIS_METAL_HITS_GROUND_2] = hge->Effect_Load("144038_SOUNDDOGS_Me metal_hit_2.mp3");
		sounds[SOUND_DEBRIS_METAL_HITS_GROUND_MAX] = hge->Effect_Load("144038_SOUNDDOGS_Me metal_hit_3.mp3");
		sounds[SOUND_DEBRIS_SANDBAG_FALL_HIT_1] = hge->Effect_Load("348682_SOUNDDOGS_BA sandbag sandbag_thud.mp3");
		sounds[SOUND_SUPPRESSED_EXPLOSION_1] = hge->Effect_Load("253107_SOUNDDOGS_De explosion.mp3");
		sounds[SOUND_TREE_FALL_1] = hge->Effect_Load("tree_fall1.wav");
		sounds[SOUND_TREE_FALL_2] = hge->Effect_Load("tree_fall2.wav");
		sounds[SOUND_NAPALM_EXPLOSION_1] = hge->Effect_Load("192257_SOUNDDOGS_EX napalm 1.mp3");
		sounds[SOUND_NAPALM_EXPLOSION_2] = hge->Effect_Load("192213_SOUNDDOGS_EX napalm 2.mp3");
		sounds[SOUND_NAPALM_EXPLOSION_3] = hge->Effect_Load("192215_SOUNDDOGS_EX napalm 3.mp3");
		sounds[SOUND_FIGHTER_PLANE_FLY_BY] = hge->Effect_Load("319228_SOUNDDOGS_AI fighter fly 2.mp3");
		sounds[SOUND_MISSILE_FLY_1] = hge->Effect_Load("322520_SOUNDDOGS_MI missile.mp3");
		sounds[SOUND_MISSILE_FLY_2] = hge->Effect_Load("322520_SOUNDDOGS_MI missile.mp3");
		sounds[SOUND_MISSILE_FLY_3] = hge->Effect_Load("322520_SOUNDDOGS_MI missile.mp3");
		sounds[SOUND_MISSILE_EXPLOSION_1] = hge->Effect_Load("big exp 4b.mp3");
		sounds[SOUND_MISSILE_EXPLOSION_2] = hge->Effect_Load("big exp 4b.mp3");
		sounds[SOUND_MISSILE_EXPLOSION_3] = hge->Effect_Load("big exp 4b.mp3");
		if (!(sounds[SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_1] = hge->Effect_Load("s3.mp3"))) MessageBox(NULL, "ei onnannut ar2", "i", MB_OK);
		if (!(sounds[SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_2] = hge->Effect_Load("s3.mp3"))) MessageBox(NULL, "ei onnannut ar2", "i", MB_OK);
		if (!(sounds[SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_3] = hge->Effect_Load("s3.mp3"))) MessageBox(NULL, "ei onnannut ar2", "i", MB_OK);
		if (!(sounds[SOUND_FOOT_SOLDIER_MACHINE_GUN_FIRING_1] = hge->Effect_Load("machine_gun3.mp3"))) MessageBox(NULL, "ei onnannut mg1", "i", MB_OK);
		if (!(sounds[SOUND_FOOT_SOLDIER_MACHINE_GUN_FIRING_2] = hge->Effect_Load("machine_gun3.mp3"))) MessageBox(NULL, "ei onnannut mg2", "i", MB_OK);
		if (!(sounds[SOUND_FOOT_SOLDIER_AT_WEAPON_FIRING] = hge->Effect_Load("at_weapon3.mp3"))) MessageBox(NULL, "ei onnannut at_weapon.mp3", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_1] = hge->Effect_Load("459859_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut1", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_2] = hge->Effect_Load("459861_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut2", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_3] = hge->Effect_Load("459863_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut3", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_4] = hge->Effect_Load("140765_SOUNDDOGS_Ma.mp3")); else ; // MessageBox(NULL, "ei onnannut4", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_5] = hge->Effect_Load("459867_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut5", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_6] = hge->Effect_Load("459868_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut6", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_7] = hge->Effect_Load("459871_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut7", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_8] = hge->Effect_Load("459868_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut8", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_9] = hge->Effect_Load("459867_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut9", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_10] = hge->Effect_Load("221024_SOUNDDOGS_hu.mp3")); else ; // MessageBox(NULL, "ei onnannut10", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_11] = hge->Effect_Load("459863_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut11", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_12] = hge->Effect_Load("459861_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut12", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_13] = hge->Effect_Load("459859_SOUNDDOGS_De.mp3")); else ; // MessageBox(NULL, "ei onnannut13", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_14] = hge->Effect_Load("432394_SOUNDDOGS_Sc.mp3")); else ; // MessageBox(NULL, "ei onnannut14", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_15] = hge->Effect_Load("432396_SOUNDDOGS_Sc.mp3")); else ; // MessageBox(NULL, "ei onnannut15", "i", MB_OK);
		if (sounds[SOUND_FOOT_SOLDIER_DEATH_16] = hge->Effect_Load("432398_SOUNDDOGS_Sc.mp3")); else ; // MessageBox(NULL, "ei onnannut16", "i", MB_OK);
		sounds[SOUND_PROJECTILE_HIT_TREE_1] = hge->Effect_Load("bullet hit wood 1 437695_SOUNDDOGS_BU.wav");
		sounds[SOUND_PROJECTILE_HIT_TREE_LAST] = hge->Effect_Load("e_bullet_wood.wav");
		sounds[SOUND_MORTAR_FIRE_1] = hge->Effect_Load("mortar_launch_1.mp3");
		sounds[SOUND_MORTAR_FIRE_2] = hge->Effect_Load("mortar_launch_2.mp3");
		sounds[SOUND_MORTAR_FIRE_LAST] = hge->Effect_Load("mortar_launch_3.wav");
		sounds[SOUND_MORTAR_WHISTLE_1] = hge->Effect_Load("mortar_grenade_fly_1.mp3");
		sounds[SOUND_MORTAR_WHISTLE_LAST] = hge->Effect_Load("mortar_grenade_fly_2.mp3");
		sounds[SOUND_MORTAR_SHELL_EXPLOSION_1] = hge->Effect_Load("GREN2.wav");
		sounds[SOUND_MORTAR_SHELL_EXPLOSION_LAST] = hge->Effect_Load("GREN3.wav");
		sounds[SOUND_PRESSURE] = hge->Effect_Load("pressure.mp3");
		sounds[SOUND_BASE_MG_FIRE_1] = hge->Effect_Load("base_mg_fire.mp3");
		sounds[SOUND_BASE_MG_FIRE_2] = hge->Effect_Load("base_mg_fire.mp3");
		sounds[SOUND_BASE_MG_FIRE_3] = hge->Effect_Load("base_mg_fire.mp3");
		sounds[SOUND_BASE_MG_FIRE_LAST] = hge->Effect_Load("base_mg_fire.mp3");
		sounds[SOUND_HAND_GRENADE_EXPLOSION] = hge->Effect_Load("GREN2.wav");
		if (!(sounds[SOUND_INTRO_MUSIC] = hge->Effect_Load("intro01b.mp3"))) MessageBox(NULL, "could not load 'intro.mp3'", "i", MB_OK);
		if (!(sounds[SOUND_INTRO_MUSIC_2] = hge->Effect_Load("intro_dbg.mp3"))) MessageBox(NULL, "could not load 'intro.mp3'", "i", MB_OK);
		if (!(sounds[SOUND_INTRO_MUSIC_3] = hge->Effect_Load("intro_dbg_sorrow.mp3"))) MessageBox(NULL, "could not load 'intro.mp3'", "i", MB_OK);
		if (!(sounds[SOUND_MENU_MUSIC] = hge->Effect_Load("263160_SOUNDDOGS_Af menu.mp3"))) MessageBox(NULL, "could not load 'menu.mp3'", "i", MB_OK);
		if (!(sounds[SOUND_LEVEL_INTRO_MUSIC_1] = hge->Effect_Load("level_intro_1 471667_SOUNDDOGS_Ba Battle Talk 63 tempo+30.mp3"))) MessageBox(NULL, "could not load 'level_intro_1 471667_SOUNDDOGS_Ba Battle Talk 63 tempo+30.mp3'", "i", MB_OK);
		if (!(sounds[SOUND_LEVEL_INTRO_MUSIC_2] = hge->Effect_Load("level_intro_2 339453_SOUNDDOGS_Un Under Surveillance.mp3"))) MessageBox(NULL, "could not load 'level_intro_2 339453_SOUNDDOGS_Un Under Surveillance.mp3'", "i", MB_OK);
		if (!(sounds[SOUND_LEVEL_INTRO_MUSIC_3] = hge->Effect_Load("level_intro_3 393574_SOUNDDOGS_Tw Twilight Zone fast.mp3"))) MessageBox(NULL, "could not load 'level_intro_3 393574_SOUNDDOGS_Tw Twilight Zone fast.mp3'", "i", MB_OK);
		if (!(sounds[SOUND_GAME_OVER_MUSIC_1] = hge->Effect_Load("390415_SOUNDDOGS_Th  The March Of Progress. G Courtie E Radford gameover.mp3"))) MessageBox(NULL, "could not load '390415_SOUNDDOGS_Th  The March Of Progress. G Courtie E Radford gameover.mp3'", "i", MB_OK);
		if (!(sounds[SOUND_VICTORY_MUSIC_1] = hge->Effect_Load("victory 339613_SOUNDDOGS_Ch Chemical Dance.mp3"))) MessageBox(NULL, "could not load 'victory 339613_SOUNDDOGS_Ch Chemical Dance.mp3'", "i", MB_OK);
	}

	//sprites
	{
		//sprites for particles
		smokeParticleSprite = new hgeSprite(particletex, 0, 3 * 32, 32, 32);
		smokeParticleSprite->SetHotSpot(32/2, 32/2);

		smokeParticleSprite2 = new hgeSprite(particletex, 32, 0, 32, 32);
		smokeParticleSprite2->SetHotSpot(32/2, 32/2);

		smokeParticleSprite3 = new hgeSprite(particletex, 32, 0, 32, 32);
		smokeParticleSprite3->SetHotSpot(32/2, 32/2);
		smokeParticleSprite3->SetBlendMode(BLEND_COLORADD | BLEND_ALPHAADD | BLEND_NOZWRITE);

		fireParticleSprite = new hgeSprite(particletex, 32, 0, 32, 32);
		fireParticleSprite->SetHotSpot(32/2, 32/2);
		fireParticleSprite->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);

		fireCloudParticleSprite = new hgeSprite(particletex, 2 * 32, 0, 32, 32);
		fireCloudParticleSprite->SetHotSpot(32/2, 32/2);
		fireCloudParticleSprite->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);

		debrisParticleSprite = new hgeSprite(particletex2, 0, 0, 32, 32);
		debrisParticleSprite->SetHotSpot(32/2, 32/2);
		debrisParticleSprite->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);

		debrisParticleSprite2 = new hgeSprite(particletex3, 0, 0, 32, 32);
		debrisParticleSprite2->SetHotSpot(32/2, 32/2);

	}

	//init particle effects
	{
		renderLoadScreen("LOADING PARTICLE EFFECTS");

		//Particle systems:

		particleSystems[PARTICLE_SYSTEM_DUST_RISE] = new hgeParticleSystem("dust_rise.psi", smokeParticleSprite);
		particleSystems[PARTICLE_SYSTEM_ASSAULT_RIFLE_FIRING_SMOKE_1] = new hgeParticleSystem("assault_rifle_fire_smoke1.psi", smokeParticleSprite2);
		particleSystems[PARTICLE_SYSTEM_CANNON_FIRING_SMOKE] = new hgeParticleSystem("tank_cannon_fire.psi", smokeParticleSprite);	
		particleSystems[PARTICLE_SYSTEM_AT_FIRING_SMOKE] = new hgeParticleSystem("at_smoke.psi", animations[ANIMATION_PARTICLE_SPRITE_SMOKE_4_LIGHT], 30.0f);	
		particleSystems[PARTICLE_SYSTEM_AT_FIRING_BACKFIRE] = new hgeParticleSystem("particle8.psi", animations[ANIMATION_PARTICLE_SPRITE_BOLT], 30.0f);
		particleSystems[PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_DIRT] = new hgeParticleSystem("hand_grenade_dirt.psi", game.smokeParticleSprite);
		particleSystems[PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_SHRAPNEL] = new hgeParticleSystem("hand_grenade_shrapnel.psi", game.smokeParticleSprite);
		particleSystems[PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_FIRE] = new hgeParticleSystem("hand_grenade_fire.psi", game.fireParticleSprite);
		particleSystems[PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_SMOKE] = new hgeParticleSystem("hand_grenade_smoke.psi", game.smokeParticleSprite);

		for (int i = 0; i < PARTICLE_SYSTEM_MAX; i++)
		{
			particleSystems[i]->info.nEmission *= (float)amountOfParticles / 100.0f;
		}
	}

	//colors:
	{
		colors[COLOR_HUD_TEXT_NORMAL] = getColor(255, 255, 255, 0);
		colors[COLOR_HUD_TEXT_CRITICAL] = getColor(255, 255, 0, 0);
		colors[COLOR_TEXT_MESSAGE_DEFENDER_SIDE] = getColor(255, 0, 255, 0); //getColor(255, 255, 200, 0);
		colors[COLOR_TEXT_MESSAGE_ATTACKER_SIDE] = getColor(255, 255, 60, 60); //getColor(255, 200, 0, 0);
	}
	
	//classes
	{
		renderLoadScreen("LOADING CLASSES");

		GameObject::init();
		BackgroundObject::init();
		CombatObject::init();
		MilitaryUnit::init();
		Base::init();
		Tank::init();
		FootSoldier::init();
		Rifleman::init();
		MachineGunner::init();
		AT_Soldier::init();
		Medic::init();
		Turret::init();
		TankMachineGun::init();
		Projectile::init();
		Sight::init();
		Effect::init();
		Tank_Carcass::init();
		Tree::init();
		Bush::init();
		Pointer::init();
		FireSupport::init();
	}

	//players
	player_1.init(Player::PLAYER_1);
	player_2.init(Player::PLAYER_2);

	hge->System_SetState(HGE_INIFILE, "Settings.ini");
	for (int i = 0; i < GameObject::NUMBER_OF_TYPES; i++)
	{
		objects.render_type_location[i] = (bool)hge->Ini_GetInt("SHOW_LOCATIONS_FOR", GameObject::typeNames[i], 1);
	}

	FireSupport fs;
	setState(INTRO);
}

void Game::renderLoadScreen(char * message)
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0xff000000);
	fnt->SetScale(0.8f);
	char mj[255] = "LOADING GAME\n";
	game.textObjects[Game::TEXT_OBJECT_MIDSCREEN]->SetText(mj);
	game.textObjects[Game::TEXT_OBJECT_MIDSCREEN]->Render();
	hge->Gfx_EndScene();
}
void Game::playIntro(void)
{
	if (!playOutGameScene())
	{
		hge->Channel_StopAll();
		setState(MENU, 0.0f, 0.0f, FADING_TIME);
	}
}

void Game::menu(void)
{
	//Take input:

	int key = 0;
	if (take_input)
		key = hge->Input_GetKey();

	if (key == HGEK_ESCAPE)
	{
		setState(QUIT_GAME);
		return;
	}
	else if (key == HGEK_UP)
	{
		menu_choice--;
		if (menu_choice < ONE_PLAYER_GAME)
			menu_choice = QUIT;
	}
	else if (key == HGEK_DOWN)
	{
		menu_choice++;
		if (menu_choice > QUIT)
			menu_choice = ONE_PLAYER_GAME;
	}
	else if ((key == HGEK_ENTER || key == HGEK_SPACE) && nextState == -1)
	{
		switch(menu_choice)
		{
		case ONE_PLAYER_GAME: 
			{
				two_player_game = false;
				setState(START_GAME, FADING_TIME, FADING_TIME, 0.0f); 
				break;
			}
		case TWO_PLAYERS_GAME: 
			{
				two_player_game = true;
				setState(START_GAME, FADING_TIME, FADING_TIME, 0.0f);
				break; 
			}
		case TRAINING:
			{
				two_player_game = true;
				setState(START_TRAINING_GAME, FADING_TIME, FADING_TIME, 0.0f);
				break;
			}
		case OPTIONS: setState(OPTIONS_MENU); break;
		case QUIT: setState(QUIT_GAME, FADING_TIME, FADING_TIME, FADING_TIME); break;
		default: break;
		}
		return;
	}
			
	//Render graphics:
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);

	float x = 90.0f;
	float y = 150.0f;
	float horiSize = 0.7f;
	float vertiSize = 0.5f;
	animations[ANIMATION_GUI_BACKGROUND_1]->RenderEx(MIDSCREENWIDTH, MIDSCREENHEIGHT, 0, horiSize, vertiSize);
	animations[ANIMATION_TITLE]->SetColor(0xff000000);
	animations[ANIMATION_TITLE]->RenderEx(370, 170, 0.0f, 1.5f, 1.5f);
	animations[ANIMATION_TITLE]->SetColor(0xffffffff);
	animations[ANIMATION_TITLE]->RenderEx(400, 150, 0.0f, 1.5f, 1.5f);
	x = 60.0f;
	y = 30.0f;
	float xSpacing = 250;
	hgeRect rect(xSpacing, 220, Game::SCREENWIDTH - xSpacing, 385);
	hge->Gfx_SetClipping(rect.x1, rect.y1, rect.x2 - rect.x1, rect.y2 - rect.y1);
	hge->Gfx_Clear(0);
	fnt->SetScale(2.8f);
	fnt->SetColor(0xffffffff);
	float textSpacing = 100.0f;
	fnt->printf(x, y, "%c 1 PLAYER GAME", (menu_choice == ONE_PLAYER_GAME)?'>':' ');
	fnt->printf(x, y += textSpacing, "%c 2 PLAYER GAME", (menu_choice == TWO_PLAYERS_GAME)?'>':' '); 
	fnt->printf(x, y += textSpacing, "%c TRAINING", (menu_choice == TRAINING)?'>':' '); 
	fnt->printf(x, y += textSpacing, "%c OPTIONS", (menu_choice == OPTIONS)?'>':' ');
	fnt->printf(x, y += textSpacing, "%c QUIT", (menu_choice == QUIT)?'>':' ');
	hge->Gfx_SetClipping();
	processScreenFading();
	hge->Gfx_EndScene();
	//Play sounds:
	if (!hge->Channel_IsPlaying(musicChannel))
		musicChannel = playMusic(sounds[SOUND_MENU_MUSIC]);
}

void Game::optionsDialog(void)
{
	bool get_out = false; //Ahnuld: "Ket aut."

	hge->System_SetState(HGE_INIFILE, "Settings.ini");
	static int type = 0;

	//Take input:
	int key = hge->Input_GetKey();

	if (key == HGEK_ESCAPE)
	{
		get_out = true;
	}
	else if (key == HGEK_UP)
	{
		options_choice--;
		if (options_choice < 0)
			options_choice = OPTIONS_CHOICE_MAX - 1;
	}
	else if (key == HGEK_DOWN)
	{
		options_choice++;
		if (options_choice > OPTIONS_CHOICE_MAX - 1)
			options_choice = 0;
	}
	else if (key == HGEK_LEFT)
	{		
		if (options_choice == START_LEVEL && previousState != MIDLEVEL_DIALOG)
		{
			hge->System_SetState(HGE_INIFILE, "Levels.ini");
			int nLevels = hge->Ini_GetInt("GENERAL", "NUMBER_OF_LEVELS", 0);
			if (startlevelNumber > 0)
				startlevelNumber--;
			else
				startlevelNumber = nLevels - 1;
			hge->System_SetState(HGE_INIFILE, "Settings.ini");
		}
		if (options_choice == OPTIONS_CHOICE_SKILL_LEVEL)
		{
			if (skillLevel > 0)
				skillLevel = (SKILL_LEVEL)(skillLevel - 1);
		}
		else if (options_choice == MUSIC_VOLUME)
		{
			if (musicVolume > 0)
				musicVolume -= 10;
			hge->Ini_SetInt("OPTIONS", "MUSIC_VOLUME", musicVolume);
			hge->Channel_SetVolume(musicChannel, musicVolume);
		}
		else if (options_choice == FX_VOLUME)
		{
			if (fxVolume > 0)
				fxVolume -= 10;
			hge->Ini_SetInt("OPTIONS", "FX_VOLUME", fxVolume);
		}
		else if (options_choice == OBJECT_SIZE && previousState != MIDLEVEL_DIALOG)
		{
			if (GameObject::objectSize > 1.0f)
				GameObject::objectSize -= 0.01f;
			else GameObject::objectSize = 1.0f;
			hge->Ini_SetFloat("OPTIONS", "OBJECT_SIZE", GameObject::objectSize);
		}

		else if (options_choice == MAX_NUMBER_OF_SHADOWS)
		{
			if (maxShadows > 0)
				maxShadows -= 1;
			else
				maxShadows = 10;
			hge->Ini_SetInt("OPTIONS", "MAX_NUMBER_OF_SHADOWS", maxShadows);
		}
		else if (options_choice == AMOUNT_OF_PARTICLES)
		{
			if (amountOfParticles > 0)
				amountOfParticles -= 5;
			else
				amountOfParticles = 100;
			hge->Ini_SetInt("OPTIONS", "AMOUNT_OF_PARTICLES", amountOfParticles);
		}
		else if (options_choice == AMOUNT_OF_BLOOD)
		{
			if (amountOfBlood > 0)
				amountOfBlood -= 5;
			else
				amountOfBlood = 0;
			hge->Ini_SetInt("OPTIONS", "AMOUNT_OF_BLOOD", amountOfBlood);
		}
	}
	else if (key == HGEK_RIGHT)
	{
		if (options_choice == START_LEVEL && previousState != MIDLEVEL_DIALOG)
		{
			hge->System_SetState(HGE_INIFILE, "Levels.ini");
			int nLevels = hge->Ini_GetInt("GENERAL", "NUMBER_OF_LEVELS", 0);
			if (startlevelNumber < nLevels - 1)
				startlevelNumber++;
			else
				startlevelNumber = 0;
			hge->System_SetState(HGE_INIFILE, "Settings.ini");
		}
		if (options_choice == OPTIONS_CHOICE_SKILL_LEVEL)
		{
			if (skillLevel < SKILL_LEVEL_MAX)
				skillLevel = (SKILL_LEVEL)(skillLevel + 1);
		}
		else if (options_choice == MUSIC_VOLUME)
		{
			if (musicVolume < 100)
				musicVolume += 10;
			hge->Ini_SetInt("OPTIONS", "MUSIC_VOLUME", musicVolume);
			hge->Channel_SetVolume(musicChannel, musicVolume);
		}
		else if (options_choice == FX_VOLUME)
		{
			if (fxVolume < 100)
				fxVolume += 10;
			hge->Ini_SetInt("OPTIONS", "VOLUME", fxVolume);
		}
		else if (options_choice == OBJECT_SIZE && previousState != MIDLEVEL_DIALOG)
		{
			if (GameObject::objectSize < 2.0f)
				GameObject::objectSize += 0.01f;
			else GameObject::objectSize = 2.0f;
			hge->Ini_SetFloat("OPTIONS", "OBJECT_SIZE", GameObject::objectSize);
		}
		else if (options_choice == MAX_NUMBER_OF_SHADOWS)
		{
			if (maxShadows < 10)
				maxShadows += 1;
			else
				maxShadows = 0;
			hge->Ini_SetInt("OPTIONS", "MAX_NUMBER_OF_SHADOWS", maxShadows);
		}
		else if (options_choice == AMOUNT_OF_PARTICLES)
		{
			if (amountOfParticles < 100)
				amountOfParticles += 5;
			else
				amountOfParticles = 0;
			hge->Ini_SetInt("OPTIONS", "AMOUNT_OF_PARTICLES", amountOfParticles);
		}
		else if (options_choice == AMOUNT_OF_BLOOD)
		{
			if (amountOfBlood < MAX_BLOOD)
				amountOfBlood += 5;
			else
				amountOfBlood = MAX_BLOOD;
			hge->Ini_SetInt("OPTIONS", "AMOUNT_OF_BLOOD", amountOfBlood);
		}
	}
	else if (key == HGEK_ENTER || key == HGEK_SPACE)
	{
		switch(options_choice)
		{
		case SHOW_MG_TRACERS:
			{
				show_mg_tracers = !show_mg_tracers;
				hge->Ini_SetInt("OPTIONS", "SHOW_MG_TRACERS", show_mg_tracers);
				break;
			}
		case SHOW_CANNON_TRACERS:
			{
				show_cannon_tracers = !show_cannon_tracers;
				hge->Ini_SetInt("OPTIONS", "SHOW_CANNON_TRACERS", show_cannon_tracers);
				break;
			}
		case FRIENDLY_FIRE:
			{
				friendly_fire = !friendly_fire;
				hge->Ini_SetInt("OPTIONS", "FRIENDLY_FIRE", friendly_fire);
				break;
			}
		case ZOOM:
			{
				zoom = !zoom;
				hge->Ini_SetInt("OPTIONS", "ZOOM", zoom);
				break;
			}
		case TIME_ADJUST:
			{
				time_adjust = !time_adjust;
				hge->Ini_SetInt("OPTIONS", "TIME_ADJUST", time_adjust);
				break;
			}
		case PLAY_SOUNDS:
			{
				play_sounds = !play_sounds;
				hge->Ini_SetInt("OPTIONS", "PLAY_SOUNDS", play_sounds);
				break;
			}
		case DISPLAY_MESSAGES:
			{
				display_messages = !display_messages;
				hge->Ini_SetInt("OPTIONS", "DISPLAY_MESSAGES", display_messages);
				break;
			}
		case MODIFY_BACKGROUND:
			{
				modify_background = !modify_background;
				hge->Ini_SetInt("OPTIONS", "MODIFY_BACKGROUND", modify_background);
				break;
			}
		case BACK:
			{
				get_out = true;
			}
		default: break;
		}
	}
	
	//Render graphics:
	hge->Gfx_BeginScene();
	
	hge->Gfx_SetClipping(100, 100, 600, 400);
	
	hge->Gfx_Clear(0x00222222);

	float hx = hge->Ini_GetFloat("OPTIONS_DIALOG", "HEADER_X", 0);
	float hy = hge->Ini_GetFloat("OPTIONS_DIALOG", "HEADER_Y", 0);

	hge->Gfx_SetTransform(hx, hy, 0, 0, 0, 4.0f, 4.0f);

	DWORD col = fnt->GetColor();
	fnt->SetColor(0x55ffffff);
	fnt->printf(hx, hy, "OPTIONS");
	fnt->SetColor(col);

	float x = hge->Ini_GetFloat("OPTIONS_DIALOG", "CHOICES_X", 0);
	float y = hge->Ini_GetFloat("OPTIONS_DIALOG", "CHOICES_Y", 0);
	optts = hge->Ini_GetFloat("OPTIONS_DIALOG", "CHOICES_SIZE ", 0);
	hge->Gfx_SetTransform(x, y, 0, 0, 0, optts, optts);

	fnt->SetScale(1.0f);
	fnt->SetColor(0xffffffff);

	float spacing = 33.0f;
	float help = y + spacing * (OPTIONS_CHOICE_MAX + 2);
	float trans = (help - Game::SCREENHEIGHT) / (OPTIONS_CHOICE_MAX);
	
	if (previousState != MIDLEVEL_DIALOG)
	{
		fnt->printf(x, (y += spacing) - options_choice * trans, "%c Start from level: %i", (options_choice == START_LEVEL)?'>':' ', startlevelNumber + 1); 	
	}
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Skill level: %s", (options_choice == OPTIONS_CHOICE_SKILL_LEVEL)?'>':' ', skillLevelNames[skillLevel]); 	
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Music Volume: %i", (options_choice == MUSIC_VOLUME)?'>':' ', musicVolume); 	
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Effects Volume: %i", (options_choice == FX_VOLUME)?'>':' ', fxVolume); 	
	if (previousState == MIDLEVEL_DIALOG)
		fnt->SetColor(0xffff5555); 
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Object Size: %.0f %%", (options_choice == OBJECT_SIZE)?'>':' ', GameObject::objectSize * 100);
	fnt->SetColor(0xffffffff);
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Show bullet tracers: %s", (options_choice == SHOW_MG_TRACERS)?'>':' ', (show_mg_tracers)? "yes":"no");
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Show cannon tracers: %s", (options_choice == SHOW_CANNON_TRACERS)?'>':' ', (show_cannon_tracers)? "yes":"no");
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Max number of shadows per object: %i", (options_choice == MAX_NUMBER_OF_SHADOWS)?'>':' ', maxShadows);
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Friendly fire: %s", (options_choice == FRIENDLY_FIRE)?'>':' ', (friendly_fire)? "yes":"no");
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Zoom (I,K,J,L,8,9,0): %s", (options_choice == ZOOM)?'>':' ', (zoom)? "yes":"no");
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Time adjustment (Mouse wheel): %s", (options_choice == TIME_ADJUST)?'>':' ', (time_adjust)? "yes":"no");
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Play sounds: %s", (options_choice == PLAY_SOUNDS)?'>':' ', (play_sounds)? "yes":"no");
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Show messages: %s", (options_choice == DISPLAY_MESSAGES)?'>':' ', (display_messages)? "yes":"no");
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Modify Background: %s", (options_choice == MODIFY_BACKGROUND)?'>':' ', (modify_background)? "yes":"no");
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Amount Of Particles: %i % (need restart)", (options_choice == AMOUNT_OF_PARTICLES)?'>':' ', amountOfParticles);
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Amount Of Blood: %i %%", (options_choice == AMOUNT_OF_BLOOD)?'>':' ', amountOfBlood);
	fnt->printf(x, (y += spacing) - options_choice * trans, "%c Back (ESC)", (options_choice == BACK)?'>':' ');

	hge->Gfx_SetTransform(0);

	hge->Gfx_SetClipping();

	hge->Gfx_EndScene();

	if (get_out)
	{		
		setState(previousState);
	}
}

void Game::startTrainingGame(void)
{
	//render
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);	
	
	fnt->SetScale(1.0f);
	fnt->SetColor(0xffffffff);
	char mj[24];
	sprintf(mj, "LOADING TRAINING LEVEL");
	game.textObjects[Game::TEXT_OBJECT_MIDSCREEN]->SetColor(0xffffffff);
	game.textObjects[Game::TEXT_OBJECT_MIDSCREEN]->SetText(mj);
	game.textObjects[Game::TEXT_OBJECT_MIDSCREEN]->Render();

	hge->Gfx_EndScene();

	//level:
	{
		levelMan.numberOfLevels = 1;
		levelMan.levelNumber = 0;
	}

	player_1.startGame();
	two_player_game = true;
	player_2.startGame();


	currentLevel = levelMan.getLevel("TRAINING");

	if (currentLevel == NULL)
	{
		char text[128];
		sprintf(text, "Could not load training level");
		MessageBox(NULL, text, "info", MB_OK);
		setState(MENU);
		return;
	}

	//Background
	objects.background = new Background(currentLevel->backgroundFileName);

	//set players
	player_1.startLevel();
	player_2.startLevel();

	//Class static functions:
	FootSoldier::levelStart();

	messageMan.init();

	char msg[5][23][128];

	string fwdKey1 = hge->Input_GetKeyName(player_1.input.tank_forward_key);
	string bckKey1 = hge->Input_GetKeyName(player_1.input.tank_backwards_key);
	string leftKey1 = hge->Input_GetKeyName(player_1.input.tank_turn_left_key);
	string rightKey1 = hge->Input_GetKeyName(player_1.input.tank_turn_right_key);
	string wpnFwdKey1 = hge->Input_GetKeyName(player_1.input.tank_turret_angle_up_key);
	string wpnBckKey1 = hge->Input_GetKeyName(player_1.input.tank_turret_angle_down_key);
	string wpnLeftKey1 = hge->Input_GetKeyName(player_1.input.tank_turret_turn_left_key);
	string wpnRightKey1 = hge->Input_GetKeyName(player_1.input.tank_turret_turn_right_key);
	string fireWpnKey1a = hge->Input_GetKeyName(player_1.input.tank_fire_cannon_key1);
	string fireWpnKey1b = hge->Input_GetKeyName(player_1.input.tank_fire_cannon_key2);
	string switchWpnKey1 = hge->Input_GetKeyName(player_1.input.tank_toggle_firemode_key);

	string fwdKey2 = hge->Input_GetKeyName(player_2.input.tank_forward_key);
	string bckKey2 = hge->Input_GetKeyName(player_2.input.tank_backwards_key);
	string leftKey2 = hge->Input_GetKeyName(player_2.input.tank_turn_left_key);
	string rightKey2 = hge->Input_GetKeyName(player_2.input.tank_turn_right_key);
	string wpnFwdKey2 = hge->Input_GetKeyName(player_2.input.tank_turret_angle_up_key);
	string wpnBckKey2 = hge->Input_GetKeyName(player_2.input.tank_turret_angle_down_key);
	string wpnLeftKey2 = hge->Input_GetKeyName(player_2.input.tank_turret_turn_left_key);
	string wpnRightKey2 = hge->Input_GetKeyName(player_2.input.tank_turret_turn_right_key);
	string fireWpnKey2 = hge->Input_GetKeyName(player_2.input.tank_fire_cannon_key1);
	string switchWpnKey2 = hge->Input_GetKeyName(player_2.input.tank_toggle_firemode_key);

	string mortarkey = "M"; //Todo: Resolve from config
	string missileKey = "V"; //Todo: Resolve from config
	string napalmKey = "N"; //Todo: Resolve from config

	sprintf(msg[0][0], "Player 1 move forward using the %s key.", fwdKey1.c_str());
	sprintf(msg[0][1], "Player 1 move backwards  using the %s key.", bckKey1.c_str());
	sprintf(msg[0][2], "Player 1 turn left using the %s key.", leftKey1.c_str());
	sprintf(msg[0][3], "Player 1 turn right using the %s key.", rightKey1.c_str());
	sprintf(msg[0][4], "Player 1 aim higher using the %s key (or use mouse).", wpnFwdKey1.c_str());
	sprintf(msg[0][5], "Player 1 aim lower using the %s key (or use mouse).", wpnBckKey1.c_str());
	sprintf(msg[0][6], "Player 1 turn weapon left using the %s key (or use mouse).", wpnLeftKey1.c_str());
	sprintf(msg[0][7], "Player 1 turn weapon right using the %s key (or use mouse).", wpnRightKey1.c_str());
	sprintf(msg[0][8], "Player 1 fire weapon using the %s key or %s", fireWpnKey1a.c_str(), fireWpnKey1b.c_str());
	sprintf(msg[0][9], "Player 1 switch weapon using the %s key.", switchWpnKey1.c_str());

	sprintf(msg[1][0], "Player 2 move forward using the %s key.", fwdKey2.c_str());
	sprintf(msg[1][1], "Player 2 move backwards  using the %s key.", bckKey2.c_str());
	sprintf(msg[1][2], "Player 2 turn left using the %s key.", leftKey2.c_str());
	sprintf(msg[1][3], "Player 2 turn right using the %s key.", rightKey2.c_str());
	sprintf(msg[1][4], "Player 2 aim higher using the %s key.", wpnFwdKey2.c_str());
	sprintf(msg[1][5], "Player 2 aim lower using the %s key.", wpnBckKey2.c_str());
	sprintf(msg[1][6], "Player 2 turn weapon left using the %s key.", wpnLeftKey2.c_str());
	sprintf(msg[1][7], "Player 2 turn weapon right using the %s key.", wpnRightKey2.c_str());
	sprintf(msg[1][8], "Player 2 fire weapon using the %s key.", fireWpnKey2.c_str());
	sprintf(msg[1][9], "Player 2 switch weapon using the %s key.", switchWpnKey2.c_str());

	sprintf(msg[2][0], "A mortar strike, when available can be ordered using the %s key.", mortarkey.c_str());
	sprintf(msg[2][1], "A missile strike, when available can be ordered using the %s key.", missileKey.c_str());
	sprintf(msg[2][2], "A napalm strike, when available can be ordered using the %s key.", napalmKey.c_str());

	sprintf(msg[3][0], "1000 points gives you an extra tank.");
	sprintf(msg[3][1], "You can collect points by decapasitating or capturing enemy troops.");
	sprintf(msg[3][2], "Capture panicking enemy soldiers by being the nearest unit around.");

	sprintf(msg[4][0], "That's all for training. Press Enter to exit.");

	int i = 0;
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 3.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 3.0f);
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 10.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 10.0f);
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 17.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 17.0f);
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 24.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 24.0f);
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 31.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 31.0f);
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 38.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 38.0f);
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 45.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 45.0f);
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 52.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 52.0f);
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 59.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 59.0f);
	showMessage(msg[0][i]  , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 66.0f);
	showMessage(msg[1][i++], COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 66.0f);
	
	showMessage(msg[2][0] , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 73.0f);
	showMessage(msg[2][1] , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 80.0f);
	showMessage(msg[2][2] , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 87.0f);

	showMessage(msg[3][0] , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 94.0f);
	showMessage(msg[3][1] , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 101.0f);
	showMessage(msg[3][2] , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 108.0f);


	showMessage(msg[4][0] , COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 115.0f);


	setState(LEVEL_RUN, 0.0f, 0.0f, FADING_TIME);
}

void Game::startGame(void)
{
	//levels:
	{
		hge->System_SetState(HGE_INIFILE, "Levels.ini");
		levelMan.numberOfLevels = hge->Ini_GetInt("GENERAL", "NUMBER_OF_LEVELS", 1);
		levelMan.levelNumber = startlevelNumber;
	}

	player_1.startGame();
	if (two_player_game)
		player_2.startGame();
	else
		player_2.gameOver = true;

	setState(LEVEL_LOAD);
}

void Game::loadLevel(void)
{
	//render
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);	
	
	fnt->SetScale(1.0f);
	fnt->SetColor(0xffffffff);
	char mj[24];
	sprintf(mj, "LOADING LEVEL %i", levelMan.levelNumber + 1);
	game.textObjects[Game::TEXT_OBJECT_MIDSCREEN]->SetColor(0xffffffff);
	game.textObjects[Game::TEXT_OBJECT_MIDSCREEN]->SetText(mj);
	game.textObjects[Game::TEXT_OBJECT_MIDSCREEN]->Render();

	hge->Gfx_EndScene();

	currentLevel = levelMan.getNextLevel();

	if (currentLevel == NULL)
	{
		char text[128];
		sprintf(text, "Could not load level %i from 'Levels.ini'", levelMan.levelNumber);
		MessageBox(NULL, text, "info", MB_OK);
		setState(MENU);
		return;
	}
	
	//work
	//Background
	objects.background = new Background(currentLevel->backgroundFileName);

	bool succeed = (objects.background->tex != 0);

	//start objects

	//first came the base
	Base * base = new Base(CombatObject::DEFENDER_SIDE, Location(400.0f, 550.0f), 0.0f);
	objects.addGameObject(base);
	currentLevel->defenderBase = base;

	//set player(s)
	player_1.startLevel();
	if (two_player_game)
		player_2.startLevel();

	//Class static functions:
	FootSoldier::levelStart();

	messageMan.init();

	if (succeed)
		setState(LEVEL_START_INTRO, 0.0f, 0.0f, FADING_TIME);
	else
		setState(MENU, 0.0f, 0.0f, FADING_TIME);
}

void Game::levelStartIntro(void)
{
	//Render graphics:
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);

	float xSpacing = 50.0f, ySpacing = 50.0f;
	hgeRect rect(xSpacing, ySpacing, Game::SCREENWIDTH - xSpacing, Game::SCREENHEIGHT - ySpacing);
	hge->Gfx_SetClipping(rect.x1, rect.y1, rect.x2 - rect.x1, rect.y2 - rect.y1);

	hge->Gfx_Clear(0xff005500);

	float horiSize = 1.6f;
	float vertiSize = 1.5f;
	animations[ANIMATION_GUI_BACKGROUND_1]->RenderEx(MIDSCREENWIDTH, MIDSCREENHEIGHT, 0, horiSize, vertiSize);

	fnt->SetScale(2.0f);
	game.textObjects[Game::TEXT_OBJECT_LEVEL_INTRO_HEADER_SHADOW]->SetColor(0xff000000);
	game.textObjects[Game::TEXT_OBJECT_LEVEL_INTRO_HEADER_SHADOW]->SetText(currentLevel->textHeader);
	game.textObjects[Game::TEXT_OBJECT_LEVEL_INTRO_HEADER_SHADOW]->Render();
	game.textObjects[Game::TEXT_OBJECT_LEVEL_INTRO_HEADER]->SetColor(0xffffffff);
	game.textObjects[Game::TEXT_OBJECT_LEVEL_INTRO_HEADER]->SetText(currentLevel->textHeader);
	game.textObjects[Game::TEXT_OBJECT_LEVEL_INTRO_HEADER]->Render();

	xSpacing = 200.0f;
	rect.Set(xSpacing, 220, Game::SCREENWIDTH - xSpacing, 480);
	hge->Gfx_SetClipping(rect.x1, rect.y1, rect.x2 - rect.x1, rect.y2 - rect.y1);

	hge->Gfx_Clear(0xff000000);

	hge->System_SetState(HGE_INIFILE, "Levels.ini");
	fnt->SetScale(hge->Ini_GetFloat(currentLevel->levelName, "START_INTRO_TEXT_SIZE", 1.0f));
	fnt->SetColor(0xffffffff);

	char st[1000];
	sprintf(st, "LOCATION:   %s\nSITUATION:\n%s\nORDERS:\n%s", currentLevel->textLocation, currentLevel->textSituation, currentLevel->textOrders);
	renderBoundedString(fnt, 50, 50, 10, 750, st);

	hge->Gfx_SetClipping();

	processScreenFading();

	hge->Gfx_EndScene();

	//Play music:
	if (!hge->Channel_IsPlaying(musicChannel))
	{
		if (currentLevel)
		{
			if (currentLevel->introMusic)
			{
				musicChannel = game.playMusic(currentLevel->introMusic, 100, false);			
			}
		}
	}

	int key = 0;
	if (take_input)
		key = hge->Input_GetKey();
	if (key == HGEK_ENTER ||
		key == HGEK_SPACE)
	{
		setState(LEVEL_RUN, FADING_TIME, FADING_TIME, FADING_TIME);
	}
}

void Game::runLevel(void)
{
	processInput();
	objects.processAI(); //process AI input
	objects.processAction(); //process suggestions for movement and action
	objects.processInteraction(); //process actual happenings and multimedia
	render();
	checkGameStatus();
}

void Game::pause(void)
{
	//Todo: Save modified background texture
	int key = hge->Input_GetKey();
	if (key == HGEK_P || key == HGEK_PAUSE)
	{
		setState(LEVEL_RUN);
	}
	hge->Gfx_BeginScene();
	hge->Gfx_SetClipping(320, 250, 160, 100);
	hge->Gfx_Clear(0);
	fnt->SetScale(5.0f);
	textObjects[TEXT_OBJECT_LEVEL_INTRO_HEADER]->SetText("PAUSE");
	textObjects[TEXT_OBJECT_LEVEL_INTRO_HEADER]->Render();
	hge->Gfx_EndScene();
}

void Game::midLevelDialog(void)
{
	//halt state changing except for selectable states
	if (nextState != MENU &&
		nextState != QUIT_GAME &&
		nextState != -1)
		stateChangeTimer += timerDelta;

	//Take input:

	//close dialog
	int key = 0;
	if (!take_input && !(nextState == LEVEL_FINISHED))
		;
	else
		key = hge->Input_GetKey();

	if (key == HGEK_ESCAPE)
	{
		setState(LEVEL_RUN);
		return;
	}
	else if (key == HGEK_UP)
	{
		midlevel_choice--;
		if (midlevel_choice < 0)
			midlevel_choice = MIDLEVEL_CHOICE_MAX - 1;
	}
	else if (key == HGEK_DOWN)
	{
		midlevel_choice++;
		if (midlevel_choice == MIDLEVEL_CHOICE_MAX)
			midlevel_choice = 0;
	}
	else if (key == HGEK_ENTER || key == HGEK_SPACE)
	{
		switch(midlevel_choice)
		{
		case BACK_TO_GAME: setState(LEVEL_RUN); break;
		case GOTO_MENU: setState(MENU, FADING_TIME, FADING_TIME, FADING_TIME); break; 
		case GOTO_OPTIONS: setState(OPTIONS_MENU); break;
		case CLOSE_GAME: setState(QUIT_GAME, FADING_TIME, FADING_TIME, FADING_TIME); break;
		default: break;
		}
		return;
	}

	//Render graphics:
	hge->Gfx_BeginScene();

	hge->Gfx_SetClipping(200, 200, 400, 200);
	hge->Gfx_Clear(0);

	float x = 250.0f;
	float y = 200.0f;

	x = 140.0f;
	y = 120.0f;

	hge->Gfx_SetTransform(x, y, 0, 0, 0, 4.0f, 4.0f);

	fnt->SetScale(0.5f);
	fnt->SetColor(0xffffffff);
	float spacing = 22.0f;
	fnt->printf(x, y, "%c BACK TO GAME", (midlevel_choice == BACK_TO_GAME)?'>':' '); 
	fnt->printf(x, y += spacing, "%c MENU", (midlevel_choice == GOTO_MENU)?'>':' ');
	fnt->printf(x, y += spacing, "%c OPTIONS", (midlevel_choice == GOTO_OPTIONS)?'>':' ');
	fnt->printf(x, y += spacing, "%c QUIT", (midlevel_choice == CLOSE_GAME)?'>':' ');

	hge->Gfx_SetTransform(0);

	hge->Gfx_SetClipping();

	processScreenFading();

	hge->Gfx_EndScene();

	//Play sounds:
	//...	
}

void Game::levelFinished(void)
{
/*
Stats to display:
 -Players:
  -Decapasitated enemy foot soldiers
   -riflemen
   -machine gunners
   -AT soldiers
   -captured 
   -medics
   -fleeing soldiers
   -overrun
  -Own side troops damaged:
   -infantry -60
   -tanks    -400
 -Per side
  -Total infantry
   -killed
   -wounded
   -captured
   -fled
  -Total tanks
   -destroyed
   -fled
  -Fire support
   -Grenades fired rounds
   -Missiles fired missiles
   -Napalm dropped 
*/

	//get input
	{
		int key = hge->Input_GetKey();

		if (key == HGEK_ENTER)
		{
			if (take_input)
			{
				if (levelMan.levelNumber < levelMan.numberOfLevels)
				{
					setState(LEVEL_LOAD, FADING_TIME, FADING_TIME, FADING_TIME);
				}
				else
				{
					setState(GAME_FINISHED, FADING_TIME, FADING_TIME, FADING_TIME);
				}
			}
		}
	}
	
	//render stats
	{
		hge->Gfx_BeginScene();
		hge->Gfx_Clear(0);
		
		fnt->SetColor(0xffffffff);
		

		char text[128];

		//header
		{
			fnt->SetScale(0.5f);
			renderCenteredText(400, 30, "BATTLE STATISTICS", fnt);
		}

		fnt->SetScale(0.45f);

		//player stats
		float playerStatY = 60.0f;
		float textY = playerStatY;
		float spacing = 15.0f;
		float extraspacing = 5.0f;

		float playerNumberXSpacing = 260.0f;
		float p1TextX = 80.0f;
		float p1NumberX = p1TextX + playerNumberXSpacing;

		//player 1 stats
		{
			renderCenteredText(200, textY, "PLAYER 1", fnt);

			//infantry
			{		
				//pluses
				fnt->printf(p1TextX, (textY += 10.0f), "Decapasitated enemy infantry"); fnt->printf(p1NumberX, textY, "%i", game.player_1.getNDecapasitatedInfantryLevel(CombatObject::ATTACKER_SIDE));
				fnt->printf(p1TextX, (textY += spacing + extraspacing), "- Riflemen [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::RIFLEMAN]); fnt->printf(p1NumberX, textY, "%i", game.player_1.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::RIFLEMAN]);
				fnt->printf(p1TextX, (textY += spacing), "- Machine gunners [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::MACHINE_GUNNER]); fnt->printf(p1NumberX, textY, "%i", game.player_1.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::MACHINE_GUNNER]);
				fnt->printf(p1TextX, (textY += spacing), "- AT soldiers [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::AT_SOLDIER]); fnt->printf(p1NumberX, textY, "%i", game.player_1.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::AT_SOLDIER]);
				fnt->printf(p1TextX, (textY += spacing), "- Captives taken [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::CAPTIVE_TAKEN]); fnt->printf(p1NumberX, textY, "%i", game.player_1.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::CAPTIVE_TAKEN]);

				//negatives			
				fnt->printf(p1TextX, (textY += spacing + extraspacing), "- Captives killed [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::CAPTIVE_KILLED]); fnt->printf(p1NumberX, textY, "%i", game.player_1.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::CAPTIVE_KILLED]);
				fnt->printf(p1TextX, (textY += spacing), "- Medics [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::MEDIC]); fnt->printf(p1NumberX, textY, "%i", game.player_1.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::MEDIC]);
			}

			//tanks
			{		
				//pluses
				fnt->printf(p1TextX, (textY += spacing + extraspacing), "Decapasitated enemy tanks [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::TANK][0]); fnt->printf(p1NumberX, textY, "%i", game.player_1.nDecapasitatedTanksLevel[CombatObject::ATTACKER_SIDE]);
			}

			//own infantry
			{
				fnt->printf(p1TextX, (textY += spacing + extraspacing + extraspacing), "Decapasitated friendly infantry [-2x]"); fnt->printf(p1NumberX, textY, "%i", game.player_1.getNDecapasitatedInfantryLevel(CombatObject::DEFENDER_SIDE));
			}

			//own tanks
			{
				fnt->printf(p1TextX, (textY += spacing + extraspacing), "Decapasitated friendly tanks [-2x]"); fnt->printf(p1NumberX, textY, "%i", game.player_1.nDecapasitatedTanksLevel[CombatObject::DEFENDER_SIDE]);
			}

			fnt->printf(p1TextX, (textY += spacing + extraspacing + extraspacing), "LEVEL SCORE:"); fnt->printf(p1NumberX, textY, "%i", game.player_1.levelScore);
			fnt->printf(p1TextX, (textY += spacing + extraspacing), "TOTAL SCORE:"); fnt->printf(p1NumberX, textY, "%i", game.player_1.score);
		}

		textY = playerStatY;
		float p2TextX = 460.0f;
		float p2NumberX = p2TextX + playerNumberXSpacing;

		//player 2 stats
		{
			renderCenteredText(600, textY, "PLAYER 2", fnt);

			//infantry
			{		
				//pluses
				fnt->printf(p2TextX, (textY += 10.0f), "Decapasitated enemy infantry"); fnt->printf(p2NumberX, textY, "%i", game.player_2.getNDecapasitatedInfantryLevel(CombatObject::ATTACKER_SIDE));
				fnt->printf(p2TextX, (textY += spacing + extraspacing), "- Riflemen [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::RIFLEMAN]); fnt->printf(p2NumberX, textY, "%i", game.player_2.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::RIFLEMAN]);
				fnt->printf(p2TextX, (textY += spacing), "- Machine gunners [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::MACHINE_GUNNER]); fnt->printf(p2NumberX, textY, "%i", game.player_2.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::MACHINE_GUNNER]);
				fnt->printf(p2TextX, (textY += spacing), "- AT soldiers [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::AT_SOLDIER]); fnt->printf(p2NumberX, textY, "%i", game.player_2.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::AT_SOLDIER]);
				fnt->printf(p2TextX, (textY += spacing), "- Captives taken [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::CAPTIVE_TAKEN]); fnt->printf(p2NumberX, textY, "%i", game.player_2.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::CAPTIVE_TAKEN]);

				//negatives			
				fnt->printf(p2TextX, (textY += spacing + extraspacing), "- Captives killed [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::CAPTIVE_KILLED]); fnt->printf(p2NumberX, textY, "%i", game.player_2.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::CAPTIVE_KILLED]);
				fnt->printf(p2TextX, (textY += spacing), "- Medics [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::FOOT_SOLDIER][Player::MEDIC]); fnt->printf(p2NumberX, textY, "%i", game.player_2.nDecapasitatedInfantryLevel[CombatObject::ATTACKER_SIDE][Player::MEDIC]);
			}

			//tanks
			{		
				//pluses
				fnt->printf(p2TextX, (textY += spacing + extraspacing), "Decapasitated enemy tanks [%i]", game.scores[CombatObject::ATTACKER_SIDE][GameObject::TANK][0]); fnt->printf(p2NumberX, textY, "%i", game.player_2.nDecapasitatedTanksLevel[CombatObject::ATTACKER_SIDE]);
			}

			//own infantry
			{
				fnt->printf(p2TextX, (textY += spacing + extraspacing + extraspacing), "Decapasitated friendly infantry [-2x]"); fnt->printf(p2NumberX, textY, "%i", game.player_2.getNDecapasitatedInfantryLevel(CombatObject::DEFENDER_SIDE));
			}

			//own tanks
			{
				fnt->printf(p2TextX, (textY += spacing + extraspacing), "Decapasitated friendly tanks [-2x]"); fnt->printf(p2NumberX, textY, "%i", game.player_2.nDecapasitatedTanksLevel[CombatObject::DEFENDER_SIDE]);
			}

			fnt->printf(p2TextX, (textY += spacing + extraspacing + extraspacing), "LEVEL SCORE:"); fnt->printf(p2NumberX, textY, "%i", game.player_2.levelScore);
			fnt->printf(p2TextX, (textY += spacing + extraspacing), "TOTAL SCORE:"); fnt->printf(p2NumberX, textY, "%i", game.player_2.score);
		}

		CombatObject::SIDE att = CombatObject::ATTACKER_SIDE;
		CombatObject::SIDE def = CombatObject::DEFENDER_SIDE;
		float infStatX = 90;
		float infNumberX = infStatX + 150;
		float tankStatX = 290;
		float tankNumberX = tankStatX + 150;
		float fireSupportStatX = 490.0f;
		float fireSupportNumberX = fireSupportStatX + 200;

		//enemy stats
		{
			textY = 330.0f;
			renderCenteredText(400, (textY), "ATTACKER TROOPS", fnt);

			float mem = textY;
			//infantry
			fnt->printf(infStatX, (textY += spacing), "Infantry"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantry(att));
			fnt->printf(infStatX, (textY += spacing + extraspacing), "- Killed"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantryKilled(att));
			fnt->printf(infStatX, (textY += spacing), "- Wounded"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantryWounded(att));
			fnt->printf(infStatX, (textY += spacing), "- Captured"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantryCaptured(att));
			fnt->printf(infStatX, (textY += spacing), "- Fled"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantryFled(att));

			textY = mem;
			//tanks
			fnt->printf(tankStatX, (textY += spacing), "Tanks"); fnt->printf(tankNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfTanks(att));
			fnt->printf(tankStatX, (textY += spacing + extraspacing), "- Destroyed"); fnt->printf(tankNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfTanksDestroyed(att));
			//fnt->printf(tankStatX, (textY += spacing), "- Fled"); fnt->printf(tankNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfTanks(att));

			textY = mem;
			//fire support
			fnt->printf(fireSupportStatX, (textY += spacing), "Fire support strikes"); fnt->printf(fireSupportNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfFireSupportStrikes(att));
			fnt->printf(fireSupportStatX, (textY += spacing + extraspacing), "- Mortar rounds fired"); fnt->printf(fireSupportNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfMortarGrenadesFired(att));
			fnt->printf(fireSupportStatX, (textY += spacing), "- Missiles fired"); fnt->printf(fireSupportNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfMissilesFired(att));
			fnt->printf(fireSupportStatX, (textY += spacing), "- Napalm bombs dropped"); fnt->printf(fireSupportNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfNapalmBombsDropped(att));			

		}

		//your troops stats
		{
			textY = 450.0f;
			renderCenteredText(400, (textY), "DEFENDER TROOPS", fnt);

			float mem = textY;
			//infantry
			fnt->printf(infStatX, (textY += spacing), "Infantry"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantry(def));
			fnt->printf(infStatX, (textY += spacing + extraspacing), "- Killed"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantryKilled(def));
			fnt->printf(infStatX, (textY += spacing), "- Wounded"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantryWounded(def));
			fnt->printf(infStatX, (textY += spacing), "- Captured"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantryCaptured(def));
			fnt->printf(infStatX, (textY += spacing), "- Fled"); fnt->printf(infNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfInfantryFled(def));

			textY = mem;
			//tanks
			fnt->printf(tankStatX, (textY += spacing), "Tanks"); fnt->printf(tankNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfTanks(def));
			fnt->printf(tankStatX, (textY += spacing + extraspacing), "- Destroyed"); fnt->printf(tankNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfTanksDestroyed(def));
			//fnt->printf(tankStatX, (textY += spacing), "- Fled"); fnt->printf(tankNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfTanks(def));

			textY = mem;
			//fire support
			fnt->printf(fireSupportStatX, (textY += spacing), "Fire support strikes"); fnt->printf(fireSupportNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfFireSupportStrikes(def));
			fnt->printf(fireSupportStatX, (textY += spacing + extraspacing), "- Mortar rounds fired"); fnt->printf(fireSupportNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfMortarGrenadesFired(def));
			fnt->printf(fireSupportStatX, (textY += spacing), "- Missiles fired"); fnt->printf(fireSupportNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfMissilesFired(def));
			fnt->printf(fireSupportStatX, (textY += spacing), "- Napalm bombs dropped"); fnt->printf(fireSupportNumberX, textY, "%i", game.currentLevel->getStatistics_numberOfNapalmBombsDropped(def));			

		}

		processScreenFading();
		hge->Gfx_EndScene();
	}
}

void Game::gameOver(void)
{
	if (!playOutGameScene())
	{
		hge->Channel_StopAll();
		setState(MENU, 0.0f, 0.0f, FADING_TIME);
	}
}

void Game::quit(void)
{
	stateTimer -= hge->Timer_GetDelta();
	if (hge->Input_GetKey() || stateTimer <= 0.0f)
	{
		freeGameResources();		
		application_close_flag = true;
		return;
	}

	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0x0000FF);

	float horiSize = 0.3f;
	float vertiSize = 0.3f;
	animations[ANIMATION_GUI_BACKGROUND_1]->RenderEx(MIDSCREENWIDTH, 255, 0, horiSize, vertiSize);

	fnt->SetScale(0.5f);
	fnt->SetColor(0xffffffff);

	char * msg = "YOU CHOOSE TO QUIT\nBase Defense:\nA Game made by Arcci productions.\nDesign, programming, art,\nphoto editing, and sound editing\nby: Arhippa Pirtola"; //\nThis is not commercial software.\nAny rights to any material presented\nbelongs to the material creator\nThe creator of this software does not proclaim any rights to any sounds or photos,\nthe rights of which belong to the right beholders.";
	game.textObjects[Game::TEXT_OBJECT_QUIT_GAME_TEXT_BOX_SHADOW]->SetColor(0xff000000);
	game.textObjects[Game::TEXT_OBJECT_QUIT_GAME_TEXT_BOX_SHADOW]->SetText(msg);
	game.textObjects[Game::TEXT_OBJECT_QUIT_GAME_TEXT_BOX_SHADOW]->Render();
	game.textObjects[Game::TEXT_OBJECT_QUIT_GAME_TEXT_BOX]->SetText(msg);
	game.textObjects[Game::TEXT_OBJECT_QUIT_GAME_TEXT_BOX]->Render();

	processScreenFading();

	hge->Gfx_EndScene();
}

void Game::setTimerDelta(void)
{
	if (time_adjust)
	{
		int key = hge->Input_GetKey();

		if (timeAdjust > 0.1f)
			timeAdjust += (float)hge->Input_GetMouseWheel() / 10.0f;
		else
			timeAdjust += (float)hge->Input_GetMouseWheel() / 100.0f;

		if (timeAdjust < 0.0f)
			timeAdjust = 0.0f;
		if (timeAdjust == 0.11f)
			timeAdjust = 0.1f;

		timerDelta = hge->Timer_GetDelta() * timeAdjust;
	}
	else
		timerDelta = hge->Timer_GetDelta();
}

void Game::processInput(void)
{
	int key = hge->Input_GetKey();

	//Common player keys
	if (key == HGEK_ESCAPE)
	{
		setState(MIDLEVEL_DIALOG);
		return;
	}
	if (key == HGEK_ENTER && training)
	{
		setState(MENU, FADING_TIME, FADING_TIME, FADING_TIME);
		return;
	}
	else if (key == HGEK_P || key == HGEK_PAUSE)
	{
		setState(PAUSED);
		return;
	}
	else if (key == HGEK_M)
	{
		currentLevel->playerFireSupportCall(&objects, FireSupport::MORTAR_STRIKE);
	}
	else if (key == HGEK_N)
	{
		currentLevel->playerFireSupportCall(&objects, FireSupport::NAPALM_STRIKE);
	}
	else if (key == HGEK_V)
	{
		currentLevel->playerFireSupportCall(&objects, FireSupport::MISSILE_STRIKE);
	}
	if (zoom)
	{
		zoomInput(key);
	}

	currentLevel->runScript(&objects);

	Input::getEvent();

	player_1.processPlayer();

	if (two_player_game)
		player_2.processPlayer();
}

void Game::render(void)
{
	//Rendering
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);

	hge->Gfx_SetTransform(SCREENWIDTH / 2.0f, SCREENHEIGHT / 2.0f, dpx, dpy, 0.0f, zoomLevel, zoomLevel);

	objects.render();

	player_1.renderHUD();
	
	if (two_player_game)		
		player_2.renderHUD();

	if (currentLevel->defenderBase)
		currentLevel->defenderBase->renderStats();

	hudfont->SetScale(0.5f);
	hudfont->SetColor(colors[COLOR_HUD_TEXT_NORMAL]);
	int mortar = currentLevel->numberOfFireSupportLeft[CombatObject::DEFENDER_SIDE][FireSupport::MORTAR_STRIKE];
	int missile = currentLevel->numberOfFireSupportLeft[CombatObject::DEFENDER_SIDE][FireSupport::MISSILE_STRIKE];
	int napalm = currentLevel->numberOfFireSupportLeft[CombatObject::DEFENDER_SIDE][FireSupport::NAPALM_STRIKE];
	hudfont->printf(255.0f, 570.0f, "Mortar strikes: %i, Missile strikes: %i, Napalm strikes: %i", mortar, missile, napalm);

	messageMan.render();

	renderDebugText();

	processScreenFading();

	hge->Gfx_EndScene();
}

void Game::checkGameStatus(void)
{
	currentLevel->checkVictory();

	if (!training && //Not the tutorial level
		currentLevel->levelFinished && 
		nextState != LEVEL_FINISHED &&
		nextState != GAME_OVER)
	{
		//getting the level victory string
		hge->System_SetState(HGE_INIFILE, "Levels.ini");
		char levelName[16];
		sprintf(levelName, "LEVEL_%i", currentLevel->levelNumber);
		std::string victoryText = hge->Ini_GetString(levelName, "VICTORY_TEXT", "Victory!");
		float msgDelay = randFloat(1.0f, 3.0f);
		showMessage(victoryText.c_str(), COLOR_TEXT_MESSAGE_DEFENDER_SIDE, msgDelay);
		setState(LEVEL_FINISHED, STATE_CHANGE_TIME_LEVEL_FINISHED + msgDelay, FADING_TIME, FADING_TIME);		
	}

	//is game over?
	else if (((player_1.gameOver && player_2.gameOver) || currentLevel->defenderBase->shield <= 0) && nextState != GAME_OVER)
	{
		setState(GAME_OVER, STATE_CHANGE_TIME_GAME_OVER, FADING_TIME, 0.0f);		
	}
}

void Game::gameFinished(void)
{
	if (!playOutGameScene())
	{
		hge->Channel_StopAll();
		setState(MENU, 0.0f, 0.0f, FADING_TIME);
	}
}

//private functions:
void Game::freeLevelResources(void)
{
	if (currentLevel)
	{
		delete currentLevel; 
		currentLevel = NULL;
	}
	objects.release();
	messageMan.clear();
}

void Game::freeGameResources(void)
{
	Base::release();
	Tank::release();
	FootSoldier::release();
	Rifleman::release();
	MachineGunner::release();
	AT_Soldier::release();
	Medic::release();
	Turret::release();
	TankMachineGun::release();
	Projectile::release();
	Sight::release();
	Effect::release();
	Tank_Carcass::release();
	Tree::release();
	Bush::release();
	Pointer::release();
	FireSupport::release();
	MilitaryUnit::release();
	CombatObject::release();
	BackgroundObject::release();
	GameObject::release();

	delete outGameScene;

	hge->Texture_Free(particletex);
	hge->Texture_Free(particletex2);
	hge->Texture_Free(particletex3);
	delete smokeParticleSprite; smokeParticleSprite = NULL;
	delete smokeParticleSprite2; smokeParticleSprite2 = NULL;
	delete smokeParticleSprite3; smokeParticleSprite3 = NULL;
	delete fireParticleSprite; fireParticleSprite = NULL;
	delete fireCloudParticleSprite; fireCloudParticleSprite = NULL;
	delete debrisParticleSprite; debrisParticleSprite = NULL;
	delete debrisParticleSprite2; debrisParticleSprite2 = NULL;

	for (int i = 0; i < ANIMATION_MAX; i++)
	{
		delete animations[i];
		animations[i] = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		hge->Texture_Free(textures[i]);
	}
	for (int i = 0; i < SOUND_MAX; i++)
	{
		hge->Effect_Free(sounds[SOUND_MAX]);
	}
	for (int i = 0; i < PARTICLE_SYSTEM_MAX; i++)
	{
		delete particleSystems[i];
		particleSystems[i] = NULL;
	}

	//text objects
	for (int i = 0; i < TEXT_OBJECT_MAX; i++)
	{
		delete textObjects[i];
		textObjects[i] = NULL;
	}
}

void Game::renderDebugText(void)
{
	hudfont->SetColor(0xFFFFFF00);
	hudfont->SetScale(0.5f);
	if (!show_debug_text)
		return;

	if (list_all_objects)
		objects.listAll();

	int spacing = 12;
	float nexty = 40;

	Tank * tank = game.player_1.tank;

	int att = CombatObject::ATTACKER_SIDE;
	int def = CombatObject::DEFENDER_SIDE;

	hudfont->SetColor(colors[COLOR_HUD_TEXT_NORMAL]);
	hudfont->printf(11, 11, "FPS = %i", hge->Timer_GetFPS());
	hudfont->printf(11, 22, "time = %.2f", currentLevel->levelTime);

	hudfont->printf(10.0f, nexty += spacing, "projectilesBeginIndex: %i", objects.projectilesBeginIndex); //level overall

	//tactical info debugging
	{
		//tanks
		//hudfont->printf(10.0f, nexty += spacing, "numberOfTanks: %i", currentLevel->numberOfTanks[att]); //level overall
		//hudfont->printf(10.0f, nexty += spacing, "numberOfTanksLeft: %i", currentLevel->numberOfTanksLeft[att]); //level left
		//hudfont->printf(10.0f, nexty += spacing, "numberOfTanksAtArea: %i", currentLevel->numberOfTanksAtArea[att]); //in level now
		////infantry
		//hudfont->printf(10.0f, nexty += spacing, "numberOfAttFootSoldiers: %i", currentLevel->numberOfFootSoldiers[att]);
		//hudfont->printf(10.0f, nexty += spacing, "numberOfDefFootSoldiers: %i", currentLevel->numberOfFootSoldiers[def]);
		hudfont->printf(10.0f, nexty += spacing, "numberOfFootSoldiersLeft[att]: %i", currentLevel->numberOfFootSoldiersLeft[att]);
		hudfont->printf(10.0f, nexty += spacing, "numberOfFootSoldiersLeft[def]: %i", currentLevel->numberOfFootSoldiersLeft[def]);
		hudfont->printf(10.0f, nexty += spacing, "numberOfFootSoldiersAtArea: %i", currentLevel->numberOfFootSoldiersAtArea[att]);
		hudfont->printf(10.0f, nexty += spacing, "numberOfFootSoldiersAtArea: %i", currentLevel->numberOfFootSoldiersAtArea[def]);
		//////hudfont->printf(10.0f, nexty += spacing, "numberOfFootSoldiersActiveAtArea: %i", currentLevel->numberOfFootSoldiersActiveAtArea[att]);
		////fire support
		//hudfont->printf(10.0f, nexty += spacing, "numberOfFireSupport: %i", currentLevel->numberOfFireSupport[att]);
		//hudfont->printf(10.0f, nexty += spacing, "numberOfFireSupportLeft: %i", currentLevel->numberOfFireSupportLeft;
	}

	//hudfont->printf(10.0f, nexty += spacing, "x = %.2f, y = %.2f", game.objects.tank->x, game.objects.tank->y);
	//hudfont->printf(10.0f, nexty += spacing, "direction = %.2f RAD", game.objects.tank->direction);
	//hudfont->printf(10.0f, nexty += spacing, "turret x = %.0f, turret y = %.0f", game.objects.tank->turret.x, game.objects.tank->turret.y);
	//hudfont->printf(10.0f, nexty += spacing, "FPS = %i", hge->Timer_GetFPS());
	//hudfont->printf(10.0f, nexty += spacing, "Cannon load time = %i", (int)(hge->Timer_GetTime() - game.objects.tank->cannonLoadTimer));
	//hudfont->printf(10.0f, nexty += spacing, "Tank body direction = %f", game.objects.tank->direction);
	//hudfont->printf(10.0f, nexty += spacing, "Tank turret direction = %f", game.objects.tank->turret.direction);
	//hudfont->printf(10.0f, nexty += spacing, "Tank mg direction = %f", game.objects.tank->tankMG.direction);
	//hudfont->printf(10.0f, nexty += spacing, "Tank dx = %f, dy = %f", game.objects.tank->dx, game.objects.tank->dy);
	//hudfont->printf(10.0f, nexty += spacing, "Tank speed vector length = %f", sqrt(pow(game.objects.tank->dx, 2.0f) + pow(game.objects.tank->dy, 2.0f)));
	//hudfont->printf(10.0f, nexty += spacing, "Tank mg rel x = %f, tank mg rel y = %f", -game.objects.tank->turret.x + game.objects.tank->tankMG.x, -game.objects.tank->turret.y + game.objects.tank->tankMG.y);
	//hudfont->printf(10.0f, nexty += spacing, "timerDelta = %f", timerDelta);
	//hudfont->printf(10.0f, nexty += spacing, "GameObjects in game: %i", GameObject::numberOfObjects);
	//hudfont->printf(10.0f, nexty += spacing, "levelPassingTimer = %f", levelPassTimer);
	//hudfont->printf(10.0f, nexty += spacing, "timeAdjust = %f", timeAdjust);
	//hudfont->printf(10.0f, nexty += spacing, "zoomLevel = %f", zoomLevel);
	//hudfont->printf(10.0f, nexty += spacing, "fire started in level: = %s", currentLevel->levelCombatStarted?"true":"false");
	hudfont->printf(10.0f, nexty += spacing, "level time: %.0f", currentLevel->levelTime);
	
	//hudfont->printf(10.0f, nexty += spacing, "GameObject size in memory = %i", sizeof(GameObject));
	//hudfont->printf(10.0f, nexty += spacing, "Tank size in memory = %i", sizeof(Tank));
	//hudfont->printf(10.0f, nexty += spacing, "FootSoldier size in memory = %i", sizeof(FootSoldier));
	//hudfont->printf(10.0f, nexty += spacing, "Projectile size in memory = %i", sizeof(Projectile));
	//hudfont->printf(10.0f, nexty += spacing, "Corpse size in memory = %i", sizeof(Corpse));
	//hudfont->printf(10.0f, nexty += spacing, "Tank_Carcass size in memory = %i", sizeof(Tank_Carcass));
	//hudfont->printf(10.0f, nexty += spacing, "Effect size in memory = %i", sizeof(Effect));
	//hudfont->printf(10.0f, nexty += spacing, "hgeAnimation size in memory = %i", sizeof(hgeAnimation));
	//hudfont->printf(10.0f, nexty += spacing, "estimated overall footsoldier size size in memory = %i", sizeof(FootSoldier) + 16 * sizeof(hgeAnimation));

	//hudfont->printf(10.0f, nexty += spacing, "Tank speed = %f", game.objects.tank->speed);
	//hudfont->printf(10.0f, nexty += spacing, "trackMarkCounter = %f", game.objects.tank->trackMarkCounter);

	//hudfont->printf(10.0f, nexty += spacing, "Game Time %f", hge->Timer_GetTime());

	//hudfont->printf(10.0f, nexty += spacing, "accelerating = %i, moving = %i, deaccelerating = %i", tank->accelerating?1:0, tank->moving?1:0, tank->deaccelerating?1:0);

	/*
	//sound debugging:
	hudfont->printf(10.0f, nexty += spacing, "tank_idle_playing = %i", tank->tank_idle_playing);
	hudfont->printf(10.0f, nexty += spacing, "tank_accelerate_playing = %i", tank->tank_accelerate_playing);
	hudfont->printf(10.0f, nexty += spacing, "tank_move_playing = %i", tank->tank_move_playing);
	hudfont->printf(10.0f, nexty += spacing, "tank_deaccelerate_playing = %i", tank->tank_deaccelerate_playing?1:0);
	hudfont->printf(10.0f, nexty += spacing, "tank_deaccelerate_sound_timer = %f", tank->tank_deaccelerate_sound_timer);
	//hudfont->printf(10.0f, nexty += spacing, "");
	*/
	/*
	//firemode debugging
	if (tank)
	{
		hudfont->printf(10.0f, nexty += spacing, "tank firemode = %s", (tank->tank_firemode == tank->TANK_FIREMODE_CANNON)?"TANK_FIREMODE_CANNON":"TANK_FIREMODE_MG");
		hudfont->printf(10.0f, nexty += spacing, "counter = %i", tank->counter);
	}*/
	//hudfont->printf(10.0f, nexty += spacing, "objects in GameObjectcollection=%i", objects.objects.size() );

	//hudfont->printf(10.0f, nexty += spacing, "currentLevel->foots=%i, tank=%i", currentLevel->numberOfAttackerFootSoldiersLeft, currentLevel->numberOfAttackerTanksLeft );

	//return;

	//Turret * turret;

	//if (tank)
	//	turret = &(tank->turret);

	//cannon sight debugging
	//hudfont->printf(10.0f, nexty += spacing, "cannon sight x = %f, cannon sight y = %f", tank->cannonSight.x, tank->cannonSight.y);
	//hudfont->printf(10.0f, nexty += spacing, "cannon sight visible = %i", tank->cannonSight.visible);
	//hudfont->printf(10.0f, nexty += spacing, "cannon sight direction = %f", tank->cannonSight.direction);

	//mg sight debugging
	//hudfont->printf(10.0f, nexty += spacing, "mg sight visible = %i", tank->MGSight.visible);

	//projectile goal reaching debugging

	/*
	if (tank->cannonProj)
	{
		hudfont->printf(10.0f, nexty += spacing, "cannon sight x = %.2f, y = %.2f", tank->cannonSight.x, tank->cannonSight.y);
		hudfont->printf(10.0f, nexty += spacing, "cannon proj goalX = %.2f, goalY = %.2f", tank->cannonProj->goalX, tank->cannonProj->goalY);
		hudfont->printf(10.0f, nexty += spacing, "cannon proj x = %.2f, y = %.2f",  tank->cannonProj->x, tank->cannonProj->y);
	}*/

	/*
	//tank shell smoke debugging:
	if (tank->cannonProj)
	{
//		hudfont->printf(10.0f, nexty += spacing, "lippu = %i", tank->cannonProj->lippu);
		//hudfont->printf(10.0f, nexty += spacing, "lippu = %i", tank->cannonProj->updeitit);
	}*/

	/*
	//ENEMY TANK DBG
	//enemy tank appearance debugging
	hudfont->printf(10.0f, nexty += spacing, "levelTime = %f", currentLevel->levelTime);
	hudfont->printf(10.0f, nexty += spacing, "tanks alive = %i", Tank::tanks);
	//enemy tank basic movement debugging
	if (currentLevel->firstAttackerTank)
	{
		hudfont->printf(10.0f, nexty += spacing, "currentLevel->firstAttackerTank->tank_forward = %i", currentLevel->firstAttackerTank->tank_forward);
		//e.t. sight visibility debugging
		hudfont->printf(10.0f, nexty += spacing, "firsttank cs.visible = %i, mgs.visible = %i", currentLevel->firstAttackerTank->cannonSight.visible, currentLevel->firstAttackerTank->MGSight.visible);
	}*/

	//collision detection debugging:
	/*
	if (tank)
	{
		hudfont->printf(10.0f, nexty += spacing, "p1 tank colliding = %s", tank->colliding? "TRUE":"FALSE");
		hudfont->printf(10.0f, nexty += spacing, "p1 tank dx = %f, dy = %f", tank->dx, tank->dy);
	
		hudfont->printf(10.0f, nexty += spacing, "can't turn left = %i, right = %i", tank->cant_turn_left, tank->cant_turn_right);
		hudfont->printf(10.0f, nexty += spacing, "turning left = %i, right = %i", tank->turning_left, tank->turning_right);
		hudfont->printf(10.0f, nexty += spacing, "process action called = %i, colliding called = %i", tank->processactioncalls, tank->collisioncalls);
		hudfont->printf(10.0f, nexty += spacing, "p1 tank speed = %f", tank->speed);
		hudfont->printf(10.0f, nexty += spacing, "collisions: front: %i, back: %i, side: %i", tank->collisionToFront, tank->collisionToBack, tank->collisionToSide);
		if (tank->othertank)
		{
		//	hudfont->printf(10.0f, nexty += spacing, "ot tank dx = %f, dy = %f", tank->othertank->dx, tank->othertank->dy);
			hudfont->printf(10.0f, nexty += spacing, "ot tank speed = %f", tank->othertank->speed);
		}
	}*/

	//shell hit tank dbg:
	//enemy tank AI dbg:
	/*
	Tank * t = currentLevel->firstAttackerTank;
	if (t)
	{
		//hudfont->printf(10.0f, nexty += spacing, "distance to drive = %f", t->distance_to_drive);
		//hudfont->printf(10.0f, nexty += spacing, "distance driven = %f", t->distance_driven);
		//hudfont->printf(10.0f, nexty += spacing, "defender tank in sight = %i", t->target_tank_in_sight);
		//hudfont->printf(10.0f, nexty += spacing + 2, "sight x= %f, y = %f", t->cannonSight.x, t->cannonSight.y);
		//hudfont->printf(10.0f, nexty += spacing + 2, "directionForTurretRotation = %f", t->directionForTurretRotation);
		//hudfont->printf(10.0f, nexty += spacing + 2, "dist= %f", t->dist);
		//hudfont->printf(10.0f, nexty += spacing + 20, "vert = %i, hori = %i", t->vertical_angle_right, t->horizontal_angle_right);
		//hudfont->printf(10.0f, nexty += spacing + 20, "sightDist = %f", t->sightDist);
		//hudfont->printf(10.0f, nexty += spacing + 20, "p1 reappearance timer = %f, gameover = %i, waiting foreapp = %i", player_1.reappearanceTimer, player_1.gameOver, player_1.waiting_for_reappearance);

		//if (t->targetTank)
		{
			//hudfont->printf(10.0f, nexty += spacing, "target tank = %s", t->targetTank->dbgName);
			//hudfont->printf(10.0f, nexty += spacing, "target tank = %s", t->targetTank->dbgName);
		}
		float _x, _y;
		hge->Input_GetMousePos(&_x, &_y);
		t->x = _x;
		t->y = _y;
	}
*/
	//Foot soldier debugging:
	//hudfont->printf(10.0f, nexty += spacing, "objects in game: %i", GameObject::numberOfObjects);

/*
	FootSoldier * s = currentLevel->firstAttackerFootSoldier;
	if (s)
	{
		
		//hudfont->printf(10.0f, nexty += spacing, "foot soldier x = %f, y = %f", s->x, s->y);
		//float hox, hoy; s->image->GetHotSpot(&hox, &hoy);
		//hudfont->printf(10.0f, nexty += spacing, "foot soldier hotspot x = %f, y = %f", hox, hoy);
		//hudfont->printf(10.0f, nexty += spacing, "fot soldier animation frame number = %i", s->image->GetFrame());
		//hudfont->printf(s->x, s->y + 10, "%f, %f", hox, hoy);
		//hudfont->printf(10.0f, nexty += spacing, "fs advancing = %i", s->advancing);
		//if (s->getEnemyTarget())
		//	hudfont->printf(10.0f, nexty += spacing, "fs enemyTargetDir = %f", s->enemyTargetDirection);
		//hudfont->printf(10.0f, nexty += spacing, "fs dir = %f", s->direction);
		//hudfont->printf(10.0f, nexty += spacing, "fs deathscreamtimert = %f", s->screamTimer);
		//hudfont->printf(10.0f, nexty += spacing, "fs dir = %f", s->direction);
		//hudfont->printf(10.0f, nexty += spacing, "fs dir2 = %f", s->direction2);
		//hudfont->printf(10.0f, nexty += spacing, "moving timer = %f", s->movingTimer);
		//hudfont->printf(10.0f, nexty += spacing, "imgframe= %i, framecount= %i", s->image->GetFrame(), s->image->GetFrames());
		//hudfont->printf(10.0f, nexty += spacing, "clipAmmo= %i", s->clipAmmo);
		//hudfont->printf(10.0f, nexty += spacing, "framenumber= %i", s->image->GetFrame());
		hudfont->printf(10.0f, nexty += spacing, "aimingTimer= %f", s->aimingTimer);
		
		//Debugging objectDirection() functionality

		hge->Input_GetMousePos(&_x, &_y);
		s->x = _x;
		s->y = _y;
/*
		//set direction:
		GameObject * e = s->getEnemyTarget();
		if (e)
		{			
			float a = e->x - s->x;
			float b = e->y - s->y;
			float tangent = b / a;
			float angle = atan(tangent); 
			if (a < 0) angle += M_PI;
			else if (b < 0) angle += ::PI_TIMES_TWO;
			s->direction = angle;
			hge->Gfx_RenderLine(0.0f, 0.0f, e->x, e->y);
			hudfont->printf(10.0f, nexty += spacing, "a = %f", a);
			hudfont->printf(10.0f, nexty += spacing, "b = %f", b);
			hudfont->printf(10.0f, nexty += spacing, "enemy target direction = %f", angle);
		}
		
		//s->x = 400.0f;
		//s->y = 100.0f;
		
	}

	FootSoldier * d = currentLevel->firstDefenderFootSoldier;
	if (d)
	{
		//hudfont->printf(10.0f, nexty += spacing, "foot soldier x = %f, y = %f", d->x, d->y);
		//float hox, hoy; d->image->GetHotSpot(&hox, &hoy);
		//hudfont->printf(10.0f, nexty += spacing, "foot soldier hotspot x = %f, y = %f", hox, hoy);
		//hudfont->printf(10.0f, nexty += spacing, "fot soldier animation frame number = %i", d->image->GetFrame());
		//hudfont->printf(d->x, d->y + 10, "%f, %f", hox, hoy);
		//hudfont->printf(10.0f, nexty += spacing, "fs advancing = %i", d->advancing);
		//hudfont->printf(10.0f, nexty += spacing, "fs enemyTargetDir = %f", d->enemyTargetDirection);
		//hudfont->printf(10.0f, nexty += spacing, "fs dir = %f", d->direction);
		//hudfont->printf(10.0f, nexty += spacing, "fs deathscreamtimert = %f", d->screamTimer);
		//hudfont->printf(10.0f, nexty += spacing, "fs dir = %f", d->direction);
		//hudfont->printf(10.0f, nexty += spacing, "fs dir2 = %f", d->direction2);
		//hudfont->printf(10.0f, nexty += spacing, "moving timer = %f", d->movingTimer);
		//hudfont->printf(10.0f, nexty += spacing, "imgframe= %i, framecount= %i", d->image->GetFrame(), d->image->GetFrames());
		//hudfont->printf(10.0f, nexty += spacing, "clipAmmo= %i", d->clipAmmo);
		//hudfont->printf(10.0f, nexty += spacing, "framenumber= %i", d->image->GetFrame());
		//hudfont->printf(10.0f, nexty += spacing, "dfs aimingTimer= %f", d->aimingTimer);

		//if (d->getEnemyTarget())
			//hge->Gfx_RenderLine(d->x, d->y, d->getEnemyTarget()->x, d->getEnemyTarget()->y);
		//d->x = 400.0f;
		//d->y = 500.0f;

		//debuggind direction management
		float _x, _y;
		hge->Input_GetMousePos(&_x, &_y);
		d->x = _x;
		d->y = _y;
	}


/*
	static float tim = 0.0f;
	static int num = 0;

	tim += timerDelta;

	if (tim > 0.1f)
	{
		num = randInt(1, 7);
		tim = 0.0f;
	}
*/
	//hudfont->printf(10.0f, nexty += spacing, "ran int: %i", num);

///*	random number generation testing:
//	hudfont->printf(10.0f, nexty += spacing, "random number = %i", num);
//	hudfont->printf(10.0f, nexty += spacing, "blendmode: %i", game.player_1.tank->tankFireMGParticleSystem->info.sprite->GetBlendMode());
//
//	checking out what game.objects.objects has eaten.

//	hudfont->printf(10.0f, nexty += spacing, "amount of projectiles = %i", Projectile::amount);
//
//	hudfont->printf(10.0f, nexty += spacing, "mac cspherec = %i", Projectile::maxCSphereCount);
//
//	*/hudfont->printf(10.0f, nexty += spacing, "");

	//Foot soldier placing debugging:
	//hudfont->printf(10.0f, nexty += spacing, "numberOfAttackerFootSoldiersAtArea = %i", currentLevel->numberOfAttackerFootSoldiersAtArea);
	//hudfont->printf(10.0f, nexty += spacing, "numberOfAttackerFootSoldiersActiveAtArea = %i", currentLevel->numberOfAttackerFootSoldiersActiveAtArea);
	//if (currentLevel->firstAttackerFootSoldier)
	//	hudfont->printf(10.0f, nexty += spacing, "fefs x=%.2f, y=%.2fi", currentLevel->firstAttackerFootSoldier->x,currentLevel->firstAttackerFootSoldier->y);

	hudfont->printf(10.0f, nexty += spacing, "attacker soldiers take cover: %i", FootSoldier::attacker_taking_cover);

	//hudfont->SetColor(0xFFFFFFFF);
}

void Game::zoomInput(int key)
{
	float zoomSpeed = 10.0f * timerDelta;
	float speed = 800.0f * timerDelta; 

	if (hge->Input_GetKeyState(HGEK_8))
	{
		zoomLevel += zoomSpeed;
		dpx += zoomSpeed;
		dpy += zoomSpeed;
	}
	else if (hge->Input_GetKeyState(HGEK_9))
	{
		zoomLevel -= zoomSpeed;
	}
	else if (hge->Input_GetKey() == HGEK_0)
	{
		zoomLevel = 1.0f;
		dpx = 0.0f;
		dpy = 0.0f;

		hge->Gfx_BeginScene(objects.background->target);
		objects.background->image->Render(0.0f, 0.0f);
		hge->Gfx_EndScene();
	}
	else if (hge->Input_GetKey() == HGEK_ADD)
	{
		timeAdjust = 1.0f;
	}
	else if (hge->Input_GetKey() == HGEK_SUBTRACT)
	{
		timeAdjust += 1.0f;
	}
	else if (hge->Input_GetKey() == HGEK_MULTIPLY)
	{
		timeAdjust += 9.9;
	}
	if (hge->Input_GetKeyState(HGEK_I))
	{
		dpy += speed;
	}
	else if (hge->Input_GetKeyState(HGEK_K))
	{
		dpy -= speed;
	}
	if (hge->Input_GetKeyState(HGEK_J))
	{
		dpx += speed;
	}
	else if (hge->Input_GetKeyState(HGEK_L))
	{
		dpx -= speed;
	}
}

void Game::setState(int newState, Time stateChangeDelay, Time fadeOutTime, Time fadeInTime)
{
	if (stateChangeDelay > 0.0f)
	{
		take_input = false;
		fadeOutTimer = fadeOutTime;
		fadeInTimer = fadeInTime;
		stateChangeTimer = stateChangeDelay;
		nextState = (STATE)newState;
		return;
	}
	else if (fadeInTime > 0.0f)
	{
		fadeInTimer = fadeInTime;
	}
	take_input = true;
	nextState = (STATE)-1;

	switch(newState)
	{
	case GAME_LOAD:
		{
			break;
		}
	case INTRO:
		{
			hge->Channel_StopAll();
			setOutGameScene(new Intro());

			if (previousState == MIDLEVEL_DIALOG)
			{
				freeLevelResources();
			}
			break;
		}
	case MENU:
		{
			hge->Channel_StopAll();
			menu_choice = 0;
			if (state == MIDLEVEL_DIALOG)
			{
				//hge->Channel_StopAll();
				freeLevelResources();
			}
			else if (state == LEVEL_FINISHED)
			{
				freeLevelResources();
			}
			else if (state == LEVEL_LOAD)
			{
				freeLevelResources();
			}
			if (training)
			{
				freeLevelResources();
				training = false;
			}
			break;
		}
	case OPTIONS_MENU:
		{
			options_choice = 0;
			break;
		}
	case START_TRAINING_GAME:
		{
			training = true;
			hge->Channel_StopAll();
			break;
		}
	case START_GAME:
		{
			hge->Channel_StopAll();
			break;
		}
	case LEVEL_LOAD:
		{
			if (state == LEVEL_FINISHED)
			{
				freeLevelResources();
			}
			break;
		}
	case LEVEL_START_INTRO:
		{
			break;
		}
	case LEVEL_RUN:
		{
			if (state != MIDLEVEL_DIALOG &&
				state != PAUSED)
				hge->Channel_StopAll();

			hge->System_SetState(HGE_INIFILE, "Settings.ini");
			godMode = hge->Ini_GetInt("OPTIONS", "GOD_MODE", 0);

			break;
		}
	case PAUSED:
		{
			break;
		}
	case MIDLEVEL_DIALOG:
		{
			midlevel_choice = 0;
			break;
		}
	case LEVEL_FINISHED:
		{
			//set player(s)
			player_1.finishLevel();
			if (two_player_game)
				player_2.finishLevel();
			hge->Channel_StopAll();			
			break;
		}
	case GAME_FINISHED:
		{
			setOutGameScene(new VictoryScene());

			hge->Channel_StopAll();
			freeLevelResources();

			//set player(s)
			player_1.finishGame();
			if (two_player_game)
				player_2.finishGame();
			break;
		}
	case GAME_OVER:
		{
			setOutGameScene(new GameOverScene());
			hge->Channel_StopAll();
			freeLevelResources();
			break;
		}
	case QUIT_GAME:
		{
			stateTimer = QUIT_GAME_TIME;
			if (state == MENU)
			{
			}
			if (state == MIDLEVEL_DIALOG)
			{
				freeLevelResources();
			}
			break;
		}
	default:
		{
			break;
		}
	}
	previousState = state;
	state = (STATE)newState;
}

void Game::processStateFlow(void)
{
	if (stateChangeTimer > 0.0f)
	{
		stateChangeTimer -= timerDelta;

		if (stateChangeTimer <= 0.0f)
			setState(nextState);
	}
}

HEFFECT Game::playSound(HEFFECT effect, int volume, float pitch, bool loop)
{
	if (!game.play_sounds)
		volume = 0;
	else
		volume = (int)((float)volume * ((float)fxVolume / 100.0f));
	return hge->Effect_PlayEx(effect, volume, 0, pitch, loop);
}

HEFFECT Game::playMusic(HEFFECT effect, int volume, bool loop)
{
	if (!game.play_sounds)
		volume = 0;
	else
		volume = (int)((float)volume * ((float)musicVolume / 100.0f));
	return hge->Effect_PlayEx(effect, volume, 0, 1.0f, loop);
}

void Game::processScreenFading(void)
{

	if (fadeOutTimer > stateChangeTimer)
	{
		float floatValue = 255.0f * (1.0f - fadeOutTimer / FADING_TIME);
		int intValue = (int)floatValue;
		animations[ANIMATION_FADE_FILTER]->SetColor(getColor(intValue, 0, 0, 0));
		animations[ANIMATION_FADE_FILTER]->Render(Game::SCREENWIDTH / 2, Game::SCREENHEIGHT / 2);
		fadeOutTimer -= timerDelta;
	}
	if (fadeInTimer > 0.0f && fadeOutTimer <= 0.0f)
	{
		float floatValue = 255.0f * fadeInTimer / FADING_TIME;
		int intValue = (int)floatValue;
		animations[ANIMATION_FADE_FILTER]->SetColor(getColor(intValue, 0, 0, 0));
		animations[ANIMATION_FADE_FILTER]->Render(Game::SCREENWIDTH / 2, Game::SCREENHEIGHT / 2);
		fadeInTimer -= timerDelta;
	}
}

void Game::setOutGameScene(OutGameScene * slideShow)
{
	if (outGameScene)
	{
		delete outGameScene;
	}
	outGameScene = slideShow;
}

bool Game::playOutGameScene(void)
{
	int key = hge->Input_GetKey();

	bool playing = true;

	if (outGameScene)
	{
		if (!outGameScene->play() ||
			hge->Input_GetKey() == HGEK_ENTER) //DEBUG
		{
			playing = false;		
		}
	}
	else
		playing = false;	
	
	return playing;
}

void Game::showMessage(const char * message, int colorIndex, Time delay)
{
	if (display_messages)
		messageMan.addMessage(message, colors[colorIndex], delay);
}
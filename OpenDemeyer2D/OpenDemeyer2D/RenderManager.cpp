#include "OD2pch.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"

#include "SDL_image.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdexcept>
#include <gl/glew.h>
#include <gl/wglew.h>

#include "OpenDemeyer2D.h"

int GetOpenGLDriverIndex()
{
	auto openglIndex = -1;
	const auto driverCount = SDL_GetNumRenderDrivers();
	for (auto i = 0; i < driverCount; i++)
	{
		SDL_RendererInfo info;
		if (!SDL_GetRenderDriverInfo(i, &info))
			if (!strcmp(info.name, "opengl"))
				openglIndex = i;
	}
	return openglIndex;
}

void RenderManager::Init(SDL_Window* window)
{
	m_Window = window;

	// Create OpenGL context 
	m_pContext = SDL_GL_CreateContext(window);
	if (m_pContext == nullptr)
	{
		std::cerr << "Core::Initialize( ), error when calling SDL_GL_CreateContext: " << SDL_GetError() << std::endl;
		return;
	}
	
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW " << SDL_GetError() << std::endl;
	}

	// Set the swap interval for the current OpenGL context,
	// synchronize it with the vertical retrace
	//if (window.isVSyncOn)
	{
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			std::cerr << "Core::Initialize( ), error when calling SDL_GL_SetSwapInterval: " << SDL_GetError() << std::endl;
			return;
		}
	}
	//else
	{
		//SDL_GL_SetSwapInterval(0);
	}

	SDL_GL_GetDrawableSize(window, &windowResWidth, &windowResHeight);

	// Set the Projection matrix to the identity matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set up a two-dimensional orthographic viewing region.
	gluOrtho2D(0, windowResWidth, 0, windowResHeight); // y from bottom to top

	// Set the viewport to the client window area
	// The viewport is the rectangular region of the window where the image is drawn.
	glViewport(0, 0, windowResWidth, windowResHeight);

	// Set the Modelview matrix to the identity matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Enable color blending and use alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;			// Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;			// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;				// Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// Enable Multi-Viewport / Platform Windows

	ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext());
	ImGui_ImplOpenGL3_Init("#version 130");


	m_RenderTexture = RESOURCES.CreateRenderTexture(windowResWidth, windowResHeight);

	ENGINE.GetSettings().GetData(OD_KEEP_ASPECT_RATIO_EDITOR, m_bKeepAspectRatio);
}

void RenderManager::Render() const
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_Window);
	ImGui::NewFrame();

	SetRenderTarget(m_RenderTexture); {
	glPushMatrix();
		glClear(GL_COLOR_BUFFER_BIT);
		glScalef(1, -1, 1);
		glTranslatef(0, -static_cast<float>(windowResHeight), 0);

		SCENES.Render();

	glPopMatrix();
	} SetRenderTargetScreen();

	SCENES.RenderImGui();
	ENGINE.RenderStats();
	ENGINE.RenderSettings();
	RenderImGuiGameWindow();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(m_Window);
}

void RenderManager::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(m_pContext);
}

void RenderManager::RenderTexture(const std::shared_ptr<Texture2D>& texture, eRenderAlignMode renderMode, const SDL_FRect* srcRect) const
{
	float width  = (srcRect) ? srcRect->w : static_cast<float>(texture->GetWidth()) ;
	float height = (srcRect) ? srcRect->h : static_cast<float>(texture->GetHeight());

	// Vertex coordinates for centered orientation
	float vertexLeft	{ -0.5f };
	float vertexBottom	{ -0.5f };
	float vertexRight	{ 0.5f  };
	float vertexTop		{ 0.5f  };

	if (renderMode != eRenderAlignMode::centered)
	{
		vertexLeft   =	(static_cast<Uint8>(renderMode) & 0b1 ) ? -1.f : 0.f;
		vertexRight  =	(static_cast<Uint8>(renderMode) & 0b1 ) ? 0.f  : 1.f;
		vertexTop    =	(static_cast<Uint8>(renderMode) & 0b10) ? -1.f : 0.f;
		vertexBottom =	(static_cast<Uint8>(renderMode) & 0b10) ? 0.f  : 1.f;
	}

	vertexLeft   *= width ;
	vertexRight  *= width ;
	vertexTop    *= height;
	vertexBottom *= height;

	// Texture coordinates
	float textLeft	 { 0 };
	float textRight	 { 1 };
	float textBottom { 1 };
	float textTop	 { 0 };
	
	if (srcRect)
	{
		textLeft	= srcRect->x / texture->GetWidth();
		textRight	= (srcRect->x + srcRect->w) / texture->GetWidth();
		textTop		= (srcRect->y) / texture->GetHeight();
		textBottom	= (srcRect->y + srcRect->h) / texture->GetHeight();
	}

	// Tell opengl which texture we will use
	glBindTexture(GL_TEXTURE_2D, texture->m_Id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Draw
	glEnable(GL_TEXTURE_2D);
	{
		glBegin(GL_QUADS);
		{
			glTexCoord2f(textLeft, textBottom);
			glVertex2f(vertexLeft, vertexBottom);

			glTexCoord2f(textLeft, textTop);
			glVertex2f(vertexLeft, vertexTop);

			glTexCoord2f(textRight, textTop);
			glVertex2f(vertexRight, vertexTop);

			glTexCoord2f(textRight, textBottom);
			glVertex2f(vertexRight, vertexBottom);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void RenderManager::RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos, eRenderAlignMode renderMode, const SDL_FRect* srcRect) const
{
	glPushMatrix();
	{
		glTranslatef(pos.x, pos.y, 0);
		RenderTexture(texture, renderMode, srcRect);
	}
	glPopMatrix();
}

void RenderManager::RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos,
	const glm::vec2& scale, eRenderAlignMode renderMode, const SDL_FRect* srcRect) const
{
	glPushMatrix();
	{
		glTranslatef(pos.x, pos.y, 0);
		glScalef(scale.x, scale.y, 1);
		RenderTexture(texture, renderMode, srcRect);
	}
	glPopMatrix();
}

void RenderManager::RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos,
	const glm::vec2& scale, float rotation, eRenderAlignMode renderMode, const SDL_FRect* srcRect) const
{
	glPushMatrix();
	{
		glTranslatef(pos.x, pos.y, 0);
		glRotatef(static_cast<GLfloat>(rotation), 0, 0, 1);
		glScalef(scale.x, scale.y, 1);
		RenderTexture(texture, renderMode, srcRect);
	}
	glPopMatrix();
}

void RenderManager::RenderImGuiGameWindow() const
{
	bool isOpen{};
	ImGui::Begin("Game", &isOpen, ImGuiWindowFlags_NoScrollbar);

	float aspectRatio = float(m_RenderTexture->GetWidth()) / float(m_RenderTexture->GetHeight());

	auto pos = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();
	size.y -= 30; // add offset

	if (m_bKeepAspectRatio)
	{
		ImVec2 newSize = { size.y * aspectRatio, size.y };
		if (newSize.x > size.x)
			newSize = { size.x, size.x / aspectRatio };
		size = newSize;
	}

#pragma warning(disable : 4312)
	ImGui::Image((ImTextureID)m_RenderTexture->GetRenderedTexture(), size);
#pragma warning(default : 4312)

	ImGui::End();
}

void RenderManager::SetRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->GetFrameBuffer());
	glViewport(0, 0, renderTarget->GetWidth(), renderTarget->GetHeight());
}

void RenderManager::SetRenderTargetScreen() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowResWidth, windowResHeight);
}

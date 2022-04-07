#include "OD2pch.h"
#include "RenderManager.h"


#include <b2_body.h>
#include <b2_circle_shape.h>
#include <b2_fixture.h>
#include <b2_polygon_shape.h>

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
#include "Scene.h"

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
	auto& settings = ENGINE.GetSettings();
	
	settings.GetData(OD_GAME_RESOLUTION_WIDTH, m_GameResWidth);
	settings.GetData(OD_GAME_RESOLUTION_HEIGHT, m_GameResHeight);
	settings.GetData(OD_KEEP_ASPECT_RATIO_EDITOR, m_bKeepAspectRatio);
	
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

	SDL_GL_GetDrawableSize(window, &m_WindowResWidth, &m_WindowResHeight);

	// Set the Projection matrix to the identity matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set up a two-dimensional orthographic viewing region.
	gluOrtho2D(0, m_GameResWidth, 0, m_GameResHeight); // y from bottom to top

	// Set the viewport to the client window area
	// The viewport is the rectangular region of the window where the image is drawn.
	glViewport(0, 0, m_GameResWidth, m_GameResHeight);

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

	int renderLayers{};
	settings.GetData(OD_RENDERER_LAYERS, renderLayers);
	m_RenderLayers.reserve(renderLayers);
	for (int i{}; i < renderLayers; ++i)
	{
		m_RenderLayers.emplace_back(RESOURCES.CreateRenderTexture(m_GameResWidth, m_GameResHeight));
	}
	m_ImGuiRenderTarget = RESOURCES.CreateRenderTexture(m_GameResWidth, m_GameResHeight);
}

void RenderManager::Render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (auto& renderTarget : m_RenderLayers)
	{
		glClearColor(0, 0, 0, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->GetFrameBuffer());
		glClear(GL_COLOR_BUFFER_BIT);
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_Window);
	ImGui::NewFrame();

	SetRenderTarget(m_RenderLayers[0]); {
	glPushMatrix();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glScalef(1, -1, 1);
		glTranslatef(0, -static_cast<float>(m_GameResHeight), 0);

		SCENES.Render();

		if (m_bShowHitboxes) RenderHitboxes();

	glPopMatrix();
	} SetRenderTargetScreen();

	SCENES.RenderImGui();
	ENGINE.RenderStats();
	ENGINE.RenderSettings();
	RenderImGuiGameWindow();
	RenderImGuiRenderInfo();
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

void RenderManager::RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos,
	const glm::vec2& scale, float rotation, const glm::vec2& pivot, const SDL_FRect* srcRect, int renderTarget) const
{
	RenderTexture(texture->GetId(), texture->GetWidth(), texture->GetHeight(), pos, scale, rotation, pivot, srcRect, renderTarget);
}

void RenderManager::RenderTexture(const std::shared_ptr<RenderTarget>& texture, const glm::vec2& pos,
	const glm::vec2& scale, float rotation, const glm::vec2& pivot, const SDL_FRect* srcRect, int renderTarget) const
{
	RenderTexture(texture->GetRenderedTexture(), texture->GetWidth(), texture->GetHeight(), pos, scale, rotation, pivot, srcRect, renderTarget);
}

void RenderManager::RenderImGuiGameWindow() const
{
	bool isOpen{};
	ImGui::Begin("Game", &isOpen, ImGuiWindowFlags_NoScrollbar);

	float aspectRatio = float(m_GameResWidth) / float(m_GameResHeight);

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

	SetRenderTarget(m_ImGuiRenderTarget);
	for (auto& renderTarget : m_RenderLayers)
	{
		RenderTexture(renderTarget, glm::vec2{ 0,renderTarget->GetHeight() }, { 1,-1 }, 0, { 1,1 });
	}
	SetRenderTargetScreen();

#pragma warning(disable : 4312)
	ImGui::Image((ImTextureID)m_ImGuiRenderTarget->GetRenderedTexture(), size);
#pragma warning(default : 4312)

	ImGui::End();
}

void RenderManager::RenderImGuiRenderInfo()
{
	ImGui::Begin("Render Information");

	float width = ImGui::GetWindowSize().x;

	//bool renderHitboxes{ m_bShowHitboxes };
	ImGui::Checkbox("Show hitboxes", &m_bShowHitboxes);

	for (size_t i{}; i < m_RenderLayers.size(); ++i)
	{
		std::string text{ "Render Target " };
		text += std::to_string(i);
		ImGui::Text(text.c_str());

		float aspectRatio = float(m_RenderLayers[i]->GetWidth()) / float(m_RenderLayers[i]->GetHeight());
		
#pragma warning(disable : 4312)
		ImGui::Image((ImTextureID)m_RenderLayers[i]->GetRenderedTexture(), { width, width / aspectRatio });
#pragma warning(default : 4312)
	}

	ImGui::End();
}

void RenderManager::RenderHitboxes() const
{
	static_assert(sizeof(b2Vec2) == sizeof(glm::vec2));

	SetRenderTarget(m_RenderLayers.back());

	auto pWorld = SCENES.GetActiveScene()->GetPhysicsWorld();

	SetColor({ 255,55,255,200 });

	for (auto pBody{ pWorld->GetBodyList() }; pBody; pBody = pBody->GetNext())
	{
		auto& position = pBody->GetPosition();
		for (auto fixture{ pBody->GetFixtureList() }; fixture; fixture = fixture->GetNext())
		{
			switch (fixture->GetType())
			{
			case b2Shape::Type::e_polygon:
				{
					auto polygon = reinterpret_cast<b2PolygonShape*>(fixture->GetShape());
					b2Vec2 positions[b2_maxPolygonVertices]{};
					for (int32 i{}; i < polygon->m_count; ++i)
					{
						positions[i] = polygon->m_vertices[i] + position;
					}
					RenderPolygon(reinterpret_cast<glm::vec2*>(positions), polygon->m_count, true);
				}
				break;
			case b2Shape::Type::e_circle:
				{
					auto circle = reinterpret_cast<b2CircleShape*>(fixture->GetShape());
					glm::vec2 center = { position.x + circle->m_p.x,position.y + circle->m_p.y };
					RenderEllipse(center, { circle->m_radius, circle->m_radius }, true);
				}
				break;
			}
		}
			
	}
}

void RenderManager::SetRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->GetFrameBuffer());
	glViewport(0, 0, renderTarget->GetWidth(), renderTarget->GetHeight());
}

void RenderManager::SetRenderTargetScreen() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_WindowResWidth, m_WindowResHeight);
}

void RenderManager::RenderTexture(GLuint glId, int w, int h, const glm::vec2& pos, const glm::vec2& scale, float rotation,
	const glm::vec2& pivot, const SDL_FRect* srcRect, int renderTarget) const
{
	if (renderTarget != -1) {
		assert(int(m_RenderLayers.size()) > renderTarget && renderTarget >= 0);
		SetRenderTarget(m_RenderLayers[renderTarget]);
	}

	glPushMatrix();
	{
		glTranslatef(pos.x, pos.y, 0);
		glRotatef(static_cast<GLfloat>(rotation), 0, 0, 1);
		glScalef(scale.x, scale.y, 1);

		{
			float width = (srcRect) ? srcRect->w : static_cast<float>(w);
			float height = (srcRect) ? srcRect->h : static_cast<float>(h);

			// Vertex coordinates for centered orientation
			float vertexLeft{ pivot.x - 1.f };
			float vertexBottom{ pivot.y - 1.f };
			float vertexRight{ pivot.x };
			float vertexTop{ pivot.y };

			vertexLeft *= width;
			vertexRight *= width;
			vertexTop *= height;
			vertexBottom *= height;

			// Texture coordinates
			float textLeft{ 0 };
			float textRight{ 1 };
			float textBottom{ 1 };
			float textTop{ 0 };

			if (srcRect)
			{
				textLeft = srcRect->x / w;
				textRight = (srcRect->x + srcRect->w) / w;
				textTop = (srcRect->y) / h;
				textBottom = (srcRect->y + srcRect->h) / h;
			}

			// Tell opengl which texture we will use
			glBindTexture(GL_TEXTURE_2D, glId);
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

	}
	glPopMatrix();

	//SetRenderTargetScreen();
}

/**
 * STATIC FUNCTIONS
 */

void RenderManager::SetColor(const SDL_Color& color)
{
	glColor4ub(color.r, color.g, color.b, color.a);
}

void RenderManager::RenderPoint(const glm::vec2& pos, float pointSize, SDL_Color* pColor)
{
	if (pColor) SetColor(*pColor);

	glPointSize(pointSize);
	glBegin(GL_POINTS);
	glVertex2f(pos.x, pos.y);
	glEnd();
}

void RenderManager::RenderLine(const glm::vec2& begin, const glm::vec2& end, float thickness, SDL_Color* pColor)
{
	if (pColor) SetColor(*pColor);

	glLineWidth(thickness);
	glBegin(GL_LINES);
	glVertex2f(begin.x, begin.y);
	glVertex2f(end.x, end.y);
	glEnd();
}

void RenderManager::RenderRect(const SDL_FRect& rect, bool filled, SDL_Color* pColor)
{
	if (pColor) SetColor(*pColor);

	glBegin(filled ? GL_POLYGON : GL_LINE_LOOP);
	glVertex2f(rect.x, rect.y);
	glVertex2f(rect.x + rect.w, rect.y);
	glVertex2f(rect.x + rect.w, rect.y + rect.h);
	glVertex2f(rect.x, rect.y + rect.h);
	glEnd();
}

void RenderManager::RenderPolygon(const std::vector<glm::vec2>& points, bool filled, SDL_Color* pColor)
{
	if (pColor) SetColor(*pColor);

	RenderPolygon(points.data(), points.size(), filled, pColor);
}

void RenderManager::RenderPolygon(const glm::vec2* points, size_t size, bool filled, SDL_Color* pColor)
{
	if (pColor) SetColor(*pColor);

	glBegin(filled ? GL_POLYGON : GL_LINE_LOOP);
	for (size_t i{}; i < size; ++i)
	{
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();
}

// SOURCE: Digital Arts and Entertainment programming 2 framework
void RenderManager::RenderEllipse(const glm::vec2& center, const glm::vec2& radii, bool filled, SDL_Color* pColor)
{
	if (pColor) SetColor(*pColor);

	float dAngle{ radii.x > radii.y ? float(M_PI / radii.x) : float(M_PI / radii.y) };

	glBegin(filled ? GL_POLYGON : GL_LINE_LOOP);
	{
		for (float angle = 0.0; angle < float(2 * M_PI); angle += dAngle)
		{
			glVertex2f(center.x + radii.x * float(cos(angle)), center.y + radii.y * float(sin(angle)));
		}
	}
	glEnd();
}

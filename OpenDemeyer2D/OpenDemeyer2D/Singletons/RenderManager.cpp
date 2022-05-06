#include "RenderManager.h"

#include "Singletons/SceneManager.h"
#include "Singletons/ResourceManager.h"

#include "SDL_image.h"

#include <stdexcept>
#include <gl/glew.h>
#include <gl/wglew.h>

#include "OpenDemeyer2D.h"
#include "EngineFiles/Scene.h"
#include "EngineFiles/ComponentBase.h"

#include "Singletons/GUIManager.h"

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

	m_Window = window;

	/**
	* OPENGL
	*/

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

	int renderLayers{};
	settings.GetData(OD_RENDERER_LAYERS, renderLayers);
	m_RenderLayers.reserve(renderLayers);
	for (int i{}; i < renderLayers; ++i)
	{
		m_RenderLayers.emplace_back(RESOURCES.CreateRenderTexture(m_GameResWidth, m_GameResHeight));
	}
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

	auto& gui = GUI;
	gui.BeginGUI();

	SetRenderTarget(m_RenderLayers[0]); {
	glPushMatrix();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glScalef(1, -1, 1);
		glTranslatef(0, -static_cast<float>(m_GameResHeight), 0);

		SCENES.Render();

		gui.RenderGUIOnGame();

	glPopMatrix();
	} SetRenderTargetScreen();

	gui.RenderGUI();
	gui.EndGUI();

	for (auto& call : m_PostDrawGLCalls)
		call();
	m_PostDrawGLCalls.clear();

	SDL_GL_SwapWindow(m_Window);
}

void RenderManager::Destroy()
{
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

void RenderManager::AddGLCallAfterDrawing(const std::function<void()>& call)
{
	m_PostDrawGLCalls.emplace_back(call);
}

void RenderManager::RenderTexture(GLuint glId, int w, int h, const glm::vec2& pos, const glm::vec2& scale, float rotation,
	const glm::vec2& pivot, const SDL_FRect* srcRect, int renderTarget) const
{
	if (renderTarget != -1) {
		assert(int(m_RenderLayers.size()) > renderTarget && renderTarget >= 0);
		SetRenderTarget(m_RenderLayers[renderTarget]);
	}

	{
		float width = (srcRect) ? srcRect->w : static_cast<float>(w);
		float height = (srcRect) ? srcRect->h : static_cast<float>(h);

		glm::vec2 vertices[4]{};

		// Vertex coordinates for centered orientation
		float vertexLeft{ pivot.x - 1.f };
		float vertexBottom{ pivot.y - 1.f };
		float vertexRight{ pivot.x };
		float vertexTop{ pivot.y };

		vertexLeft *= width * scale.x;
		vertexRight *= width * scale.x;
		vertexTop *= height * scale.y;
		vertexBottom *= height * scale.y;

		constexpr float inverse180{ 1.f / 180.f * float(M_PI) };

		float cosAngle = cos(rotation * inverse180);
		float sinAngle = sin(rotation * inverse180);

		vertices[0] = { vertexLeft * cosAngle - vertexBottom * sinAngle, vertexBottom * cosAngle + vertexLeft * sinAngle };
		vertices[1] = { vertexLeft * cosAngle - vertexTop * sinAngle, vertexTop * cosAngle + vertexLeft * sinAngle };
		vertices[2] = { vertexRight * cosAngle - vertexTop * sinAngle, vertexTop * cosAngle + vertexRight * sinAngle };
		vertices[3] = { vertexRight * cosAngle - vertexBottom * sinAngle, vertexBottom * cosAngle + vertexRight * sinAngle };

		vertices[0] += pos;
		vertices[1] += pos;
		vertices[2] += pos;
		vertices[3] += pos;

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
				glVertex2f(vertices[0].x, vertices[0].y);

				glTexCoord2f(textLeft, textTop);
				glVertex2f(vertices[1].x, vertices[1].y);

				glTexCoord2f(textRight, textTop);
				glVertex2f(vertices[2].x, vertices[2].y);

				glTexCoord2f(textRight, textBottom);
				glVertex2f(vertices[3].x, vertices[3].y);
			}
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
	}


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

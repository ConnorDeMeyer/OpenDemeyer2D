#include "PhysicsComponent.h"
#include <intrin.h>
#include <limits>

#include <b2_body.h>
#include <b2_fixture.h>
#include <b2_polygon_shape.h>
#include "Components/Transform.h"

#include "EngineFiles/Scene.h"
#include "Singletons/OpenDemeyer2D.h"
#include "EngineIO/CustomSerializers.h"

#include "imgui.h"

void PhysicsComponent::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<b2BodyDef>("Body", offsetof(PhysicsComponent, m_BodyDef));
	binder.Add<std::vector<b2FixtureDef>>("Fixtures", offsetof(PhysicsComponent, m_FixtureDefs));
	binder.Add<std::vector<std::shared_ptr<b2Shape>>>("Shapes", offsetof(PhysicsComponent, m_Shapes));
}

PhysicsComponent::~PhysicsComponent()
{
	if (GetObject()) {
		auto scene = GetObject()->GetScene();
		if (scene && m_pBody)
		{
			scene->GetPhysicsInterface()->DestroyBody(m_pBody);
		}
	}
}

void PhysicsComponent::BeginPlay()
{
	if (!m_pBody) CreateBody(m_BodyDef);
	for (size_t i{}; i < m_FixtureDefs.size(); ++i)
	{
		m_FixtureDefs[i].shape = m_Shapes[i].get();
		AddFixture(m_FixtureDefs[i]);
	}
}

void PhysicsComponent::Update(float)
{
	auto transform = GetObject()->GetTransform();
	m_pBody->SetTransform(reinterpret_cast<const b2Vec2&>(transform->GetWorldPosition()), transform->GetWorldRotation());
}

void PhysicsComponent::RenderImGui()
{
	if (ImGui::CollapsingHeader("Body"))
	{
		ImGui::Checkbox("Allow Sleep", &m_BodyDef.allowSleep);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Set this flag to false if this body should never fall asleep. Note that this increases CPU usage.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		ImGui::Checkbox("Bullet", &m_BodyDef.bullet);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Is this a fast moving body that should be prevented from tunneling through other moving bodies? Note that all bodies are prevented from tunneling through kinematic and static bodies. This setting is only considered on dynamic bodies");
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "You should use this flag sparingly since it increases processing time.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		ImGui::Checkbox("fixed Rotation", &m_BodyDef.fixedRotation);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Should this body be prevented from rotating? Useful for characters.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		ImGui::DragFloat("Angular Damping", &m_BodyDef.angularDamping, 0.05f, 0, 1);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Angular damping is use to reduce the angular velocity. The damping parameter can be larger than 1.0f but the damping effect becomes sensitive to the time step when the damping parameter is large. Units are 1/time");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		ImGui::InputFloat("Gravity Scale", &m_BodyDef.gravityScale);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Scale the gravity applied to this body.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		ImGui::DragFloat("linearDamping", &m_BodyDef.linearDamping, 0.05f, 0, 1);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Linear damping is use to reduce the linear velocity. The damping parameter can be larger than 1.0f but the damping effect becomes sensitive to the time step when the damping parameter is large. Units are 1/time");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		int bodyType{ int(m_BodyDef.type) };
		if (ImGui::Combo("Body Type", &bodyType, "static\0kinematic\0dynamic"))
		{
			m_BodyDef.type = b2BodyType(bodyType);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("The body type: static, kinematic, or dynamic. Note: if a dynamic body would have zero mass, the mass is set to one.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
	
	if (ImGui::CollapsingHeader("Fixtures"))
	{
		if (ImGui::Button("Add Fixture"))
		{
			m_FixtureDefs.emplace_back(b2FixtureDef());
			m_Shapes.emplace_back(new b2CircleShape());
		}

		for (size_t it{}; it < m_FixtureDefs.size(); ++it)
		{
			ImGui::PushID(&m_FixtureDefs[it]);

			auto& fixture = m_FixtureDefs[it];
			auto& shape = m_Shapes[it];

			if (ImGui::TreeNode("Fixture"))
			{

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete fixture"))
					{
						m_FixtureDefs.erase(m_FixtureDefs.begin() + it);
						m_Shapes.erase(m_Shapes.begin() + it);
						ImGui::CloseCurrentPopup();
						ImGui::EndPopup();
						ImGui::TreePop();
						ImGui::PopID();
						break;
					}

					ImGui::EndPopup();
				}

				// SHAPE
				constexpr int b2ShapeAmount{ 4 };

				constexpr const char* b2ShapeNames[b2ShapeAmount]
				{
					"Circle",
					"Edge",
					"Polygon",
					"Chain"
				};

				constexpr const char* b2ShapeDescriptions[b2ShapeAmount]
				{
					"A solid circle shape.",
					"A line segment (edge) shape. These can be connected in chains or loops to other edge shapes. Edges created independently are two-sided and do no provide smooth movement across junctions.",
					"A solid convex polygon. It is assumed that the interior of the polygon is to the left of each edge. Polygons have a maximum number of vertices equal to b2_maxPolygonVertices. In most cases you should not need many vertices for a convex polygon.",
					"A chain shape is a free form sequence of line segments. The chain has one-sided collision, with the surface normal pointing to the right of the edge. This provides a counter-clockwise winding like the polygon shape. Connectivity information is used to create smooth collisions."
				};

				int comboSelection{int(shape->GetType())};
				if (ImGui::BeginCombo("Shape", b2ShapeNames[comboSelection]))
				{
					for (int i{}; i < b2ShapeAmount; ++i)
					{
						if (ImGui::Selectable(b2ShapeNames[i], comboSelection == i))
						{
							if (i != int(shape->GetType()))
							{
								switch (b2Shape::Type(i))
								{
								case b2Shape::Type::e_circle:
								{
									auto circle = new b2CircleShape();
									shape = std::shared_ptr<b2Shape>(circle);
									circle->m_radius = 10;
								}
								break;
								case b2Shape::Type::e_edge:
								{
									auto edge = new b2EdgeShape();
									shape = std::shared_ptr<b2Shape>(edge);
									edge->m_vertex1 = {};
									edge->m_vertex2 = {};
								}
								break;
								case b2Shape::Type::e_polygon:
									shape = std::shared_ptr<b2Shape>(new b2PolygonShape());
									break;
								case b2Shape::Type::e_chain:
									shape = std::shared_ptr<b2Shape>(new b2ChainShape());
									break;
								}
							}
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
							ImGui::TextUnformatted(b2ShapeDescriptions[i]);
							ImGui::PopTextWrapPos();
							ImGui::EndTooltip();
						}
					}

					ImGui::EndCombo();
				}

				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted(b2ShapeDescriptions[int(shape->GetType())]);
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}

				static_assert(sizeof(b2Vec2) == sizeof(float[2])); // because of reinterpret casting between b2vec2 and float[2]

				switch (shape->GetType())
				{
				case b2Shape::Type::e_circle:
					{
					if (auto circle{ std::reinterpret_pointer_cast<b2CircleShape>(shape) })
					{
						ImGui::InputFloat2("Position", reinterpret_cast<float*>(&circle->m_p));
						ImGui::InputFloat("Radius", &circle->m_radius);
					}
					}
					break;
				case b2Shape::Type::e_edge:
					{
					if (auto edge{ std::reinterpret_pointer_cast<b2EdgeShape>(shape) })
					{
						if (ImGui::Checkbox("One Sided", &edge->m_oneSided))
						{
							if (edge->m_oneSided)
							{
								edge->SetOneSided(edge->m_vertex0, edge->m_vertex1, edge->m_vertex2, edge->m_vertex3);
							}
							else
							{
								edge->SetTwoSided(edge->m_vertex0, edge->m_vertex1);
							}
						}

						bool changed{ };
						changed = changed || ImGui::InputFloat2("Vertex0", reinterpret_cast<float*>(&edge->m_vertex0));
						changed = changed || ImGui::InputFloat2("Vertex3", reinterpret_cast<float*>(&edge->m_vertex3));
						if (edge->m_oneSided)
						{
							ImGui::InputFloat2("Vertex1", reinterpret_cast<float*>(&edge->m_vertex1));
							ImGui::InputFloat2("Vertex2", reinterpret_cast<float*>(&edge->m_vertex2));
						}
						else if (changed)
						{
							edge->SetTwoSided(edge->m_vertex0, edge->m_vertex3);
						}
					}
					}
					break;
				case b2Shape::Type::e_polygon:
					{
					if (auto polygon{ std::reinterpret_pointer_cast<b2PolygonShape>(shape) })
					{
						if (ImGui::TreeNode("Set As Box"))
						{
							static float cubeDimensions[2]{};
							static b2Vec2 position{};
							static float angle{};
							ImGui::InputFloat2("Half width & height", cubeDimensions);
							ImGui::InputFloat2("Position", reinterpret_cast<float*>(&position));
							ImGui::SliderFloat("Rotation", &angle, -360.f, 360.f);
							if (ImGui::Button("Generate Cube"))
							{
								polygon->SetAsBox(cubeDimensions[0], cubeDimensions[1], position, angle);
							}
							ImGui::TreePop();
						}
						if (ImGui::TreeNode("Set As Polygon"))
						{
							static b2Vec2 vertices[b2_maxPolygonVertices];
							static int32 count{3};
							ImGui::SliderInt("VerticeAmount", &count, 3, b2_maxPolygonVertices);
							for (int32 i{}; i < count; ++i)
							{
								ImGui::InputFloat2((std::string("vertex") + std::to_string(i)).c_str(), reinterpret_cast<float*>(&vertices[i]));
							}
							if (ImGui::Button("Generate Polygon"))
							{
								polygon->Set(vertices, count);
							}
							ImGui::TreePop();
						}

						ImGui::Text("Vertices:");
						for (int32 i{}; i < polygon->m_count; ++i)
						{
							ImGui::Text("[ %.3f , %.3f]", polygon->m_vertices[i].x, polygon->m_vertices[i].y);
						}
					}
					}
					break;
				case b2Shape::Type::e_chain:
					{
					if (auto chain{ std::reinterpret_pointer_cast<b2ChainShape*>(shape) })
					{

					}
					}
					break;
				}

				// FIXTURE

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.f);

				ImGui::Checkbox("Trigger", &fixture.isSensor);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("A sensor shape collects contact information but never generates a collision response.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}

				ImGui::DragFloat("Friction", &fixture.friction, 0.01f, 0, 1);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("The friction coefficient, usually in the range [0,1].");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}

				ImGui::DragFloat("Restitution", &fixture.restitution, 0.01f, 0, 1);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("The restitution (elasticity) usually in the range [0,1].");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}

				ImGui::InputFloat("Restitution Threshold", &fixture.restitutionThreshold);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Restitution velocity threshold, usually in m/s. Collisions above this speed have restitution applied (will bounce).");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}

				ImGui::InputFloat("Density", &fixture.density);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("The density, usually in kg/m^2.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				
				unsigned long collisionGroup{};
				_BitScanForward(&collisionGroup, fixture.filter.categoryBits);
				int collisionGrpInt{ int(collisionGroup) };
				if (ImGui::SliderInt("Collision Group", &collisionGrpInt, 1, 16))
				{
					fixture.filter.categoryBits = 0b1 << collisionGrpInt;
				}
				
				unsigned long ignoreGroup{};
				_BitScanForward(&ignoreGroup, fixture.filter.maskBits);
				int ignoreGrpInt{ int(ignoreGroup) };
				if (ImGui::SliderInt("Ignore Group", &ignoreGrpInt, 1, 16))
				{
					fixture.filter.maskBits = 0b1 << ignoreGrpInt;
				}
				
				int grpIndex{ fixture.filter.groupIndex };
				if (ImGui::InputInt("Group Index", &grpIndex, 1, 25))
				{
					fixture.filter.groupIndex = std::clamp(int16(grpIndex), std::numeric_limits<int16>::lowest(), std::numeric_limits<int16>::max());
				}


				ImGui::TreePop();
			}
			else
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete fixture"))
					{
						m_FixtureDefs.erase(m_FixtureDefs.begin() + it);
						m_Shapes.erase(m_Shapes.begin() + it);
						ImGui::CloseCurrentPopup();
						ImGui::EndPopup();
						ImGui::PopID();
						break;
					}

					ImGui::EndPopup();
				}
			}

			ImGui::PopID();
		}
	}
}

const glm::vec2& PhysicsComponent::GetLinearVelocity() const
{
	return reinterpret_cast<const glm::vec2&>(m_pBody->GetLinearVelocity());
}

void PhysicsComponent::AddBox(float halfWidth, float halfHeight, bool isSensor, const glm::vec2& center, float rotation)
{
	if (!m_pBody) return;

	b2PolygonShape boxShape{};
	boxShape.SetAsBox(halfWidth, halfHeight, { center.x, center.y }, rotation);

	b2FixtureDef def{};
	def.userData.pointer = uintptr_t(this);
	def.isSensor = isSensor;
	def.shape = &boxShape;

	m_pBody->CreateFixture(&def);
}

void PhysicsComponent::AddFixture(b2FixtureDef& fixture)
{
	fixture.userData.pointer = uintptr_t(this);
	m_pBody->CreateFixture(&fixture);
}

void PhysicsComponent::CreateBody(b2BodyDef& def)
{
	auto scene = GetScene();
	if (scene && m_pBody)
	{
		scene->GetPhysicsInterface()->DestroyBody(m_pBody);
	}

	auto& position = GetObject()->GetTransform()->GetWorldPosition();

	def.userData.pointer = reinterpret_cast<uintptr_t>(this);
	def.position.Set(position.x, position.y);

	m_pBody = GetScene()->GetPhysicsInterface()->CreateBody(def);
}

std::vector<PhysicsComponent*> PhysicsComponent::GetOverlappingComponents()
{
	if (m_pBody)
	{
		return GetScene()->GetPhysicsInterface()->GetOverlappingComponents(this);

		/*b2AABB aabb{};
		aabb.lowerBound = { b2_maxFloat, b2_maxFloat };
		aabb.upperBound = { -b2_maxFloat, -b2_maxFloat };

		for (auto it{ m_pBody->GetFixtureList() }; it; it = it->GetNext())
		{
			if (it->IsSensor())
			{
				aabb.Combine(it->GetAABB(0));
			}
		}

		std::vector<b2Fixture*> overlappingFixtures;

		class AABBQueryOverride : public b2QueryCallback
		{
		public:
			AABBQueryOverride(std::vector<b2Fixture*>* Fixtures, b2Fixture* queryFixtures)
				: m_Fixtures{Fixtures}
				, m_queryFixtures{queryFixtures}
			{}

			bool ReportFixture(b2Fixture* fixture) override
			{
				m_Fixtures->emplace_back(fixture);
				return true;
			}
		private:
			std::vector<b2Fixture*>* m_Fixtures{};
			b2Fixture* m_queryFixtures{};
		};

		AABBQueryOverride query{ &overlappingFixtures, m_pBody->GetFixtureList() };

		m_pBody->GetWorld()->QueryAABB(&query, aabb);

		return overlappingFixtures;*/
	}

	return std::vector<PhysicsComponent*>();
}

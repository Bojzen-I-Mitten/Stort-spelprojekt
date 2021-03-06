/*
	Wrapper around the debug draw provided by the Bullet physics library.
*/

#pragma once
#define _XM_NO_INTRINSICS_ 1;
#include <LinearMath\btIDebugDraw.h>

namespace thomas
{

	namespace graphics
	{
		class BulletDebugDraw : public btIDebugDraw
		{
		public:
			BulletDebugDraw();
			~BulletDebugDraw();

		public:
			virtual void drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & fromColor, const btVector3 & toColor) override;
			virtual void drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color) override;
			virtual void drawSphere(const btVector3 & p, btScalar radius, const btVector3 & color) override;
			virtual void drawTriangle(const btVector3 & a, const btVector3 & b, const btVector3 & c, const btVector3 & color, btScalar alpha) override;
			virtual void drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color) override;
			virtual void reportErrorWarning(const char* warningString) override;
			virtual void draw3dText(const btVector3 & location, const char* textString) override {};
			virtual void setDebugMode(int debugMode) override;
			virtual int getDebugMode() const override { return m_debugMode; }

		public:

		private:

		private:
			int m_debugMode;
		};
	}
}

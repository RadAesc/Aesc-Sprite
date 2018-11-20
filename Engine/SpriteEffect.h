#pragma once

#include "Colors.h"
#include "Graphics.h"

namespace SpriteEffect
{
	class Chroma
	{
	public:
		Chroma( Color c )
			:
			chroma( c )
		{}
		void operator()( Color cSrc,int xDest,int yDest,Graphics& gfx ) const
		{
			if( cSrc != chroma )
			{
				gfx.PutPixel( xDest,yDest,cSrc );
			}
		}
	private:
		Color chroma;
	};
	class Substitution
	{
	public:
		Substitution( Color c,Color s )
			:
			chroma( c ),
			sub( s )
		{}
		void operator()( Color cSrc,int xDest,int yDest,Graphics& gfx ) const
		{
			if( cSrc != chroma )
			{
				gfx.PutPixel( xDest,yDest,sub );
			}
		}
	private:
		Color chroma = Colors::Magenta;
		Color sub;
	};
	class Copy
	{
	public:
		void operator()( Color cSrc,int xDest,int yDest,Graphics& gfx )
		{
			gfx.PutPixel( xDest,yDest,cSrc );
		}
	};
	class Ghost
	{
	public:
		Ghost( Color c )
			:
			chroma( c )
		{}
		void operator()( Color src,int xDest,int yDest,Graphics& gfx ) const
		{
			if( src != chroma )
			{
				const Color dest = gfx.GetPixel( xDest,yDest );
				const Color blend =
				{
					unsigned char( ( src.GetR() + dest.GetR() ) / 2 ),
					unsigned char( ( src.GetG() + dest.GetG() ) / 2 ),
					unsigned char( ( src.GetB() + dest.GetB() ) / 2 )
				};
			}
		}
	private:
		Color chroma;
	};
	// Dissolves image by scanline and blends drawn pixels with a color,
	//  good for dying enemies I guess.
	class DissolveHalfTint
	{
	public:
		DissolveHalfTint( Color chroma,Color tint,float percent )
			:
			chroma( chroma ),
			// divide channels by 2 via shift, mask to prevent bleeding between channels
			tint_pre( ( tint.dword >> 1u ) & 0b01111111011111110111111101111111u ),
			filled( int( float( height ) * percent ) )
		{}
		void operator()( Color src,int xDest,int yDest,Graphics& gfx ) const
		{
			// height mask determines frequency of vertical dissolve sections
			if( src != chroma && ( yDest & height_mask ) < filled )
			{
				const Color blend = tint_pre.dword +
					// divide channels by 2 via shift, mask to prevent bleeding between channels
					( ( src.dword >> 1u ) & 0b01111111011111110111111101111111u );
				gfx.PutPixel( xDest,yDest,blend );
			}
		}
	private:
		Color chroma;
		Color tint_pre;
		static constexpr int height = 4;
		static constexpr int height_mask = height - 1;
		int filled;
	};
	class SubstituteFade
	{
	public:
		SubstituteFade( Color chroma,Color sub,float fadeAmount )
			:
			chroma( chroma ),
			substitution( sub ),
			fadeAmount( fadeAmount )
		{}
		void operator()( Color cSrc,int xDest,int yDest,Graphics& gfx )
		{
			if( cSrc != chroma )
			{
				gfx.PutPixel( xDest,yDest,cSrc );
				gfx.PutPixelAlpha( xDest,yDest,substitution,fadeAmount );
			}
		}
	private:
		Color chroma;
		Color substitution;
		float fadeAmount;
	};
	class BlackAndWhite
	{
	public:
		BlackAndWhite( Color chroma )
			:
			chroma( chroma )
		{}
		void operator()( Color cSrc,int xDest,int yDest,Graphics& gfx ) const
		{
			if( cSrc != chroma )
			{
				const auto val = float( cSrc.GetR() ) * 0.3f +
					float( cSrc.GetG() ) * 0.59f +
					float( cSrc.GetB() ) * 0.11f;

				const auto uval = unsigned char( val );
				gfx.PutPixel( xDest,yDest,Colors
					::MakeRGB( uval,uval,uval ) );
			}
		}
	private:
		Color chroma;
	};
	class Inverse
	{
	public:
		Inverse( Color chroma )
			:
			chroma( chroma )
		{}
		void operator()( Color cSrc,int xDest,int yDest,Graphics& gfx ) const
		{
			if( cSrc != chroma )
			{
				const auto white = Colors::White;
				auto pix = gfx.GetPixel( xDest,yDest );

				pix.SetR( white.GetR() - pix.GetR() );
				pix.SetG( white.GetG() - pix.GetG() );
				pix.SetB( white.GetB() - pix.GetB() );

				gfx.PutPixel( xDest,yDest,pix );
			}
		}
	private:
		Color chroma;
	};
}
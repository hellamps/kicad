
#ifndef __KICAD_DCSVG_H
#define __KICAD_DCSVG_H

#include <wx/dc.h>
#include <wx/wfstream.h>
#include <wx/string.h>

#define wxSVGVersion wxT( "v0101" )

#if wxCHECK_VERSION( 2, 9, 0 )

// We could do nothing, because wxWidgets 3 supports the SVG format
// (previously, it was a contribution library, not included in wxWidgets)
// However arcs are drawn as pies, and we must change it.
// Unfortunately most of functions are private, and we cannot derive
// our KicadSVGFileDCImpl from wxSVGFileDCImpl
// and just override the 2 incorrect functions
// Just wxWidget dcsvg is copied here and 2 functions are modified:
// KicadSVGFileDCImpl::DoDrawArc() and KicadSVGFileDCImpl::DoDrawEllipticArc()
// Also note: SetLogicalFunction() does not set an error in debug mode

class WXDLLIMPEXP_FWD_BASE wxFileOutputStream;
class WXDLLIMPEXP_FWD_CORE KicadSVGFileDC;

class WXDLLIMPEXP_CORE KicadSVGFileDCImpl : public wxDCImpl
{
public:
    KicadSVGFileDCImpl( KicadSVGFileDC *owner, const wxString &filename,
                     int width=320, int height=240, double dpi=72.0 );

    virtual ~KicadSVGFileDCImpl();

    bool IsOk() const { return m_OK; }

    virtual bool CanDrawBitmap() const { return true; }
    virtual bool CanGetTextExtent() const { return true; }

    virtual int GetDepth() const
    {
        wxFAIL_MSG(wxT("wxSVGFILEDC::GetDepth Call not implemented"));
        return -1;
    }

    virtual void Clear()
    {
        wxFAIL_MSG(wxT("wxSVGFILEDC::Clear() Call not implemented \nNot sensible for an output file?"));
    }

    virtual void DestroyClippingRegion()
    {
        wxFAIL_MSG(wxT("wxSVGFILEDC::void Call not yet implemented"));
    }

    virtual wxCoord GetCharHeight() const;
    virtual wxCoord GetCharWidth() const;

    virtual void SetClippingRegion(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                                   wxCoord WXUNUSED(w), wxCoord WXUNUSED(h))
    {
        wxFAIL_MSG(wxT("wxSVGFILEDC::SetClippingRegion not implemented"));
    }

    virtual void SetPalette(const wxPalette&  WXUNUSED(palette))
    {
        wxFAIL_MSG(wxT("wxSVGFILEDC::SetPalette not implemented"));
    }

    virtual void GetClippingBox(wxCoord *WXUNUSED(x), wxCoord *WXUNUSED(y),
                                wxCoord *WXUNUSED(w), wxCoord *WXUNUSED(h))
    {
        wxFAIL_MSG(wxT("wxSVGFILEDC::GetClippingBox not implemented"));
    }

    virtual void SetLogicalFunction(wxRasterOperationMode WXUNUSED(function))
    {
//        wxFAIL_MSG(wxT("KicadSVGFILEDC::SetLogicalFunction not implemented"));
    }

    virtual wxRasterOperationMode GetLogicalFunction() const
    {
        wxFAIL_MSG(wxT("wxSVGFILEDC::GetLogicalFunction() not implemented"));
        return wxCOPY;
    }

    virtual void SetBackground( const wxBrush &brush );
    virtual void SetBackgroundMode( int mode );
    virtual void SetBrush(const wxBrush& brush);
    virtual void SetFont(const wxFont& font);
    virtual void SetPen(const wxPen& pen);

private:
   virtual bool DoGetPixel(wxCoord, wxCoord, wxColour *) const
   {
       wxFAIL_MSG(wxT("wxSVGFILEDC::DoGetPixel Call not implemented"));
       return true;
   }

   virtual bool DoBlit(wxCoord, wxCoord, wxCoord, wxCoord, wxDC *,
                       wxCoord, wxCoord, wxRasterOperationMode = wxCOPY,
                       bool = 0, int = -1, int = -1);

   virtual void DoCrossHair(wxCoord, wxCoord)
   {
       wxFAIL_MSG(wxT("wxSVGFILEDC::CrossHair Call not implemented"));
   }

   virtual void DoDrawArc(wxCoord, wxCoord, wxCoord, wxCoord, wxCoord, wxCoord);

   virtual void DoDrawBitmap(const wxBitmap &, wxCoord, wxCoord, bool = false);

   virtual void DoDrawCheckMark(wxCoord x, wxCoord y, wxCoord w, wxCoord h);

   virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord w, wxCoord h);

   virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                  double sa, double ea);

   virtual void DoDrawIcon(const wxIcon &, wxCoord, wxCoord);

   virtual void DoDrawLine (wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

   virtual void DoDrawLines(int n, wxPoint points[],
                            wxCoord xoffset = 0, wxCoord yoffset = 0);

   virtual void DoDrawPoint(wxCoord, wxCoord);

   virtual void DoDrawPolygon(int n, wxPoint points[],
                              wxCoord xoffset, wxCoord yoffset,
                              wxPolygonFillMode fillStyle);

   virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h);

   virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                  double angle);

   virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                       wxCoord w, wxCoord h,
                                       double radius = 20) ;

   virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);

   virtual bool DoFloodFill(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                            const wxColour& WXUNUSED(col),
                            wxFloodFillStyle WXUNUSED(style) = wxFLOOD_SURFACE)
   {
       wxFAIL_MSG(wxT("wxSVGFILEDC::DoFloodFill Call not implemented"));
       return false;
   }

   virtual void DoGetSize(int * x, int *y) const
   {
       if ( x )
           *x = m_width;
       if ( y )
           *y = m_height;
   }

   virtual void DoGetTextExtent(const wxString& string, wxCoord *w, wxCoord *h,
                                wxCoord *descent = NULL,
                                wxCoord *externalLeading = NULL,
                                const wxFont *font = NULL) const;

   virtual void DoSetDeviceClippingRegion(const wxRegion& WXUNUSED(region))
   {
       wxFAIL_MSG(wxT("wxSVGFILEDC::DoSetDeviceClippingRegion not yet implemented"));
   }

   virtual void DoSetClippingRegion( int WXUNUSED(x),  int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height) )
   {
       wxFAIL_MSG(wxT("wxSVGFILEDC::DoSetClippingRegion not yet implemented"));
   }

   virtual void DoGetSizeMM( int *width, int *height ) const;

   virtual wxSize GetPPI() const;

   void Init (const wxString &filename, int width, int height, double dpi);

   void NewGraphics();

   void write( const wxString &s );

private:
   wxFileOutputStream *m_outfile;
   wxString            m_filename;
   int                 m_sub_images; // number of png format images we have
   bool                m_OK;
   bool                m_graphics_changed;
   int                 m_width, m_height;
   double              m_dpi;

private:
   DECLARE_ABSTRACT_CLASS(KicadSVGFileDCImpl)
};


class WXDLLIMPEXP_CORE KicadSVGFileDC : public wxDC
{
public:
    KicadSVGFileDC(const wxString& filename,
                int width = 320,
                int height = 240,
                double dpi = 72.0)
        : wxDC(new KicadSVGFileDCImpl(this, filename, width, height, dpi))
    {
    }
};

#else

class wxSVGFileDC : public wxDC
{
private:
    wxFileOutputStream* m_outfile;
    wxString            m_filename;

    //holds number of png format images we have
    int  m_sub_images;
    bool m_OK, m_graphics_changed;
    int  m_width, m_height;

    double
    m_logicalScaleX,
    m_logicalScaleY,
    m_userScaleX,
    m_userScaleY,
    m_scaleX,
    m_scaleY,
    m_OriginX,
    m_OriginY,
    m_mm_to_pix_x,
    m_mm_to_pix_y;

    bool
    m_needComputeScaleX,
    m_needComputeScaleY;         // not yet used


    bool DoGetPixel( wxCoord, wxCoord, class wxColour* ) const
    {
        wxASSERT_MSG( false, wxT( "wxSVGFILEDC::DoGetPixel Call not implemented" ) ); return true;
    };

    virtual bool DoBlit( wxCoord, wxCoord, wxCoord, wxCoord, class wxDC*,
                         wxCoord, wxCoord, int = wxCOPY, bool = 0, int = -1, int = -1 );

    void DoCrossHair( wxCoord, wxCoord )
    {
        wxASSERT_MSG( false, wxT( "wxSVGFILEDC::CrossHair Call not implemented" ) ); return;
    };

    void    DoDrawArc( wxCoord, wxCoord, wxCoord, wxCoord, wxCoord, wxCoord );

    void    DoDrawBitmap( const class wxBitmap&, wxCoord, wxCoord, bool = 0 );

    void    DoDrawCheckMark( wxCoord x, wxCoord y, wxCoord width, wxCoord height );

    void    DoDrawEllipse( wxCoord x, wxCoord y, wxCoord width, wxCoord height );

    void    DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea );

    void    DoDrawIcon( const class wxIcon&, wxCoord, wxCoord );

    void    DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 );

    void    DoDrawPoint( wxCoord, wxCoord );

    void    DoDrawPolygon( int n,
                           wxPoint points[],
                           wxCoord xoffset,
                           wxCoord yoffset,
                           int fillStyle );

    void    DoDrawRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height );

    void    DoDrawRotatedText( const wxString& text, wxCoord x, wxCoord y, double angle );

    void    DoDrawRoundedRectangle( wxCoord x,
                                    wxCoord y,
                                    wxCoord width,
                                    wxCoord height,
                                    double radius = 20 );

    void    DoDrawText( const wxString& text, wxCoord x, wxCoord y );

    bool DoFloodFill( wxCoord WXUNUSED (x), wxCoord WXUNUSED (y), const wxColour& WXUNUSED (col),
                      int WXUNUSED (style) = wxFLOOD_SURFACE )
    {
        wxASSERT_MSG( false, wxT( "wxSVGFILEDC::DoFloodFill Call not implemented" ) );
        return false;
    };

    void DoGetSize( int* x, int* y ) const
    {
        *x = m_width; *y = m_height; return;
    };

    void DoGetTextExtent( const wxString& string,
                          wxCoord* w,
                          wxCoord* h,
                          wxCoord* descent = NULL,
                          wxCoord* externalLeading = NULL,
                          wxFont* font = NULL ) const;

    void DoSetClippingRegionAsRegion( const class wxRegion& )
    {
        wxASSERT_MSG( false,
            wxT( "wxSVGFILEDC::DoSetClippingRegionAsRegion Call not yet implemented" ) ); return;
    };

    void    Init( wxString f, int Width, int Height, float dpi );

    void    NewGraphics();

#ifdef XDEV2LOG
#undef XDEV2LOG
#endif
    wxCoord XDEV2LOG( wxCoord x ) const
    {
        wxCoord new_x = x - m_deviceOriginX;

        if( new_x > 0 )
            return (wxCoord) ( (double) (new_x) / m_scaleX + 0.5 ) * m_signX + m_logicalOriginX;
        else
            return (wxCoord) ( (double) (new_x) / m_scaleX - 0.5 ) * m_signX + m_logicalOriginX;
    }


#ifdef XDEV2LOGREL
#undef XDEV2LOGREL
#endif
    wxCoord XDEV2LOGREL( wxCoord x ) const
    {
        if( x > 0 )
            return (wxCoord) ( (double) (x) / m_scaleX + 0.5 );
        else
            return (wxCoord) ( (double) (x) / m_scaleX - 0.5 );
    }


#ifdef YDEV2LOG
#undef YDEV2LOG
#endif
    wxCoord YDEV2LOG( wxCoord y ) const
    {
        wxCoord new_y = y - m_deviceOriginY;

        if( new_y > 0 )
            return (wxCoord) ( (double) (new_y) / m_scaleY + 0.5 ) * m_signY + m_logicalOriginY;
        else
            return (wxCoord) ( (double) (new_y) / m_scaleY - 0.5 ) * m_signY + m_logicalOriginY;
    }


#ifdef YDEV2LOGREL
#undef YDEV2LOGREL
#endif
    wxCoord YDEV2LOGREL( wxCoord y ) const
    {
        if( y > 0 )
            return (wxCoord) ( (double) (y) / m_scaleY + 0.5 );
        else
            return (wxCoord) ( (double) (y) / m_scaleY - 0.5 );
    }


#ifdef XLOG2DEV
#undef XLOG2DEV
#endif
    wxCoord XLOG2DEV( wxCoord x ) const
    {
        wxCoord new_x = x - m_logicalOriginX;

        if( new_x > 0 )
            return (wxCoord) ( (double) (new_x) * m_scaleX + 0.5 ) * m_signX + m_deviceOriginX;
        else
            return (wxCoord) ( (double) (new_x) * m_scaleX - 0.5 ) * m_signX + m_deviceOriginX;
    }


#ifdef XLOG2DEVREL
#undef XLOG2DEVREL
#endif
    wxCoord XLOG2DEVREL( wxCoord x ) const
    {
        if( x > 0 )
            return (wxCoord) ( (double) (x) * m_scaleX + 0.5 );
        else
            return (wxCoord) ( (double) (x) * m_scaleX - 0.5 );
    }


#ifdef YLOG2DEV
#undef YLOG2DEV
#endif
    wxCoord YLOG2DEV( wxCoord y ) const
    {
        wxCoord new_y = y - m_logicalOriginY;

        if( new_y > 0 )
            return (wxCoord) ( (double) (new_y) * m_scaleY + 0.5 ) * m_signY + m_deviceOriginY;
        else
            return (wxCoord) ( (double) (new_y) * m_scaleY - 0.5 ) * m_signY + m_deviceOriginY;
    }


#ifdef YLOG2DEVREL
#undef YLOG2DEVREL
#endif
    wxCoord YLOG2DEVREL( wxCoord y ) const
    {
        if( y > 0 )
            return (wxCoord) ( (double) (y) * m_scaleY + 0.5 );
        else
            return (wxCoord) ( (double) (y) * m_scaleY - 0.5 );
    }


    void write( const wxString& s );

public:

    wxSVGFileDC( wxString f );
    wxSVGFileDC( wxString f, int Width, int Height );
    wxSVGFileDC( wxString f, int Width, int Height, float dpi );
    ~wxSVGFileDC();


    bool CanDrawBitmap() const
    {
        return true;
    };

    bool CanGetTextExtent() const
    {
        return true;
    };

    int GetDepth() const
    {
        wxASSERT_MSG( false, wxT( "wxSVGFILEDC::GetDepth Call not implemented" ) ); return -1;
    };

    void BeginDrawing()
    {
        return;
    };

    bool Blit( wxCoord xdest,
               wxCoord ydest,
               wxCoord width,
               wxCoord height,
               wxDC* source,
               wxCoord xsrc,
               wxCoord ysrc,
               int logicalFunc = wxCOPY,
               bool useMask = false )
    {
        return DoBlit( xdest, ydest, width, height, source, xsrc, ysrc, logicalFunc, useMask );
    };

    void Clear()
    {
        wxASSERT_MSG( false,
            wxT( "wxSVGFILEDC::Clear() Call not implemented \nNot sensible for an output file?" ) );
        return;
    };

    void CrossHair( wxCoord x, wxCoord y )
    {
        DoCrossHair( x, y ); return;
    };

    virtual void ComputeScaleAndOrigin();

    void DestroyClippingRegion()
    {
        wxASSERT_MSG( false, wxT( "wxSVGFILEDC::void Call not yet implemented" ) ); return;
    };

    wxCoord DeviceToLogicalX( wxCoord x ) const;

    wxCoord DeviceToLogicalXRel( wxCoord x )  const;

    wxCoord DeviceToLogicalY( wxCoord y )  const;

    wxCoord DeviceToLogicalYRel( wxCoord y )  const;

    void DrawBitmap( const wxBitmap& bitmap, wxCoord x, wxCoord y, bool transparent )
    {
        DoDrawBitmap( bitmap, x, y, transparent ); return;
    };

    void DrawIcon( const wxIcon& icon, wxCoord x, wxCoord y )
    {
        DoDrawIcon( icon, x, y ); return;
    };

    void DoDrawLines( int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0 );

    void EndDoc()
    {
        return;
    };

    void EndDrawing()
    {
        return;
    };

    void EndPage()
    {
        return;
    };

    void FloodFill( wxCoord x, wxCoord y, wxColour* colour, int style = wxFLOOD_SURFACE )
    {
        DoFloodFill( x, y, *colour, style ); return;
    };

    wxCoord GetCharHeight() const;

    wxCoord GetCharWidth() const;

    void GetClippingBox( wxCoord* WXUNUSED (x), wxCoord* WXUNUSED (y), wxCoord* WXUNUSED (width),
                        wxCoord* WXUNUSED (height) )
    {
        wxASSERT_MSG( false, wxT( "wxSVGFILEDC::GetClippingBox Call not yet implemented" ) );
        return;
    };

    int GetLogicalFunction()
    {
        wxASSERT_MSG( false, wxT( "wxSVGFILEDC::GetLogicalFunction()  Call not implemented" ) );
        return wxCOPY;
    };

    int GetMapMode();

    bool GetPixel( wxCoord x, wxCoord y, wxColour* colour )
    {
        return DoGetPixel( x, y, colour );
    };

    void    GetUserScale( double* x, double* y ) const;

    wxCoord LogicalToDeviceX( wxCoord x )  const;

    wxCoord LogicalToDeviceXRel( wxCoord x )  const;

    wxCoord LogicalToDeviceY( wxCoord y )  const;

    wxCoord LogicalToDeviceYRel( wxCoord y )  const;

    bool Ok() const
    {
        return m_OK;
    };

    void SetAxisOrientation( bool xLeftRight, bool yBottomUp );

    void SetClippingRegion( wxCoord WXUNUSED (x), wxCoord WXUNUSED (y), wxCoord WXUNUSED (width),
                           wxCoord WXUNUSED (height) )
    {
        wxASSERT_MSG( false, wxT( "wxSVGFILEDC::SetClippingRegion Call not yet implemented" ) );
        return;
    };

    void SetPalette( const wxPalette& WXUNUSED (palette) )
    {
        wxASSERT_MSG( false, wxT( "wxSVGFILEDC::SetPalette Call not yet implemented" ) ); return;
    };

    void    SetBackground( const wxBrush& brush );

    void    SetBackgroundMode( int mode );

    void    SetBrush( const wxBrush& brush );

    void    SetFont( const wxFont& font );

    void SetLogicalFunction( int WXUNUSED (function) )
    {
        //wxASSERT_MSG (false, wxT("wxSVGFILEDC::SetLogicalFunction not implemented"));
        return;
    };

    void    SetLogicalScale( double x, double y );

    void    SetLogicalOrigin( wxCoord x, wxCoord y );

    void    SetDeviceOrigin( wxCoord x, wxCoord y );

    void    SetMapMode( int anint );

    void    SetPen( const wxPen& pen );

    void    SetUserScale( double xScale, double yScale );

    bool StartDoc( const wxString& WXUNUSED (message) )
    {
        return false;
    };

    void StartPage()
    {
        return;
    };
};

typedef wxSVGFileDC KicadSVGFileDC;

#endif  // wxCHECK_VERSION

#endif  // __KICAD_DCSVG_H

/****************************************************
 * class_module_transform_functions.cpp :
 * functions of class MODULE to handle some geometric changes:
 * move, rot ...
****************************************************/

#include "fctsys.h"
#include "wxstruct.h"
#include "common.h"
#include "trigo.h"
#include "pcbcommon.h"

#include "pcbnew.h"
#include "protos.h"

/*************************************/
int ChangeSideNumLayer( int oldlayer )
/*************************************/

/* Routine de recalcul du numero de couche lors des
 *  echanges cote cu/cmp pour les couches CU/CMP specialisees
 *  (cuivre, serigr., pate , soudure)
 */
{
    int newlayer;

    switch( oldlayer )
    {
    case COPPER_LAYER_N:
        newlayer = CMP_N; break;

    case CMP_N:
        newlayer = COPPER_LAYER_N; break;

    case SILKSCREEN_N_CU:
        newlayer = SILKSCREEN_N_CMP; break;

    case SILKSCREEN_N_CMP:
        newlayer = SILKSCREEN_N_CU; break;

    case ADHESIVE_N_CU:
        newlayer = ADHESIVE_N_CMP; break;

    case ADHESIVE_N_CMP:
        newlayer = ADHESIVE_N_CU; break;

    case SOLDERMASK_N_CU:
        newlayer = SOLDERMASK_N_CMP; break;

    case SOLDERMASK_N_CMP:
        newlayer = SOLDERMASK_N_CU; break;

    case SOLDERPASTE_N_CU:
        newlayer = SOLDERPASTE_N_CMP; break;

    case SOLDERPASTE_N_CMP:
        newlayer = SOLDERPASTE_N_CU; break;

    default:
        newlayer = oldlayer;
    }

    return newlayer;
}

/*********************************************/
static int ChangeSideMaskLayer( int masque )
/*********************************************/

/* Routine de recalcul du masque-layer lors des
 *  echanges cote cu/cmp pour les couches CU/CMP specialisees
 *  (cuivre, serigr., pate , soudure)
 */
{
    int newmasque;

    newmasque = masque & ~(CUIVRE_LAYER | CMP_LAYER |
                           SILKSCREEN_LAYER_CU | SILKSCREEN_LAYER_CMP |
                           ADHESIVE_LAYER_CU | ADHESIVE_LAYER_CMP |
                           SOLDERMASK_LAYER_CU | SOLDERMASK_LAYER_CMP |
                           SOLDERPASTE_LAYER_CU | SOLDERPASTE_LAYER_CMP |
                           ADHESIVE_LAYER_CU | ADHESIVE_LAYER_CMP);

    if( masque & CUIVRE_LAYER )
        newmasque |= CMP_LAYER;
    if( masque & CMP_LAYER )
        newmasque |= CUIVRE_LAYER;

    if( masque & SILKSCREEN_LAYER_CU )
        newmasque |= SILKSCREEN_LAYER_CMP;
    if( masque & SILKSCREEN_LAYER_CMP )
        newmasque |= SILKSCREEN_LAYER_CU;

    if( masque & ADHESIVE_LAYER_CU )
        newmasque |= ADHESIVE_LAYER_CMP;
    if( masque & ADHESIVE_LAYER_CMP )
        newmasque |= ADHESIVE_LAYER_CU;

    if( masque & SOLDERMASK_LAYER_CU )
        newmasque |= SOLDERMASK_LAYER_CMP;
    if( masque & SOLDERMASK_LAYER_CMP )
        newmasque |= SOLDERMASK_LAYER_CU;

    if( masque & SOLDERPASTE_LAYER_CU )
        newmasque |= SOLDERPASTE_LAYER_CMP;
    if( masque & SOLDERPASTE_LAYER_CMP )
        newmasque |= SOLDERPASTE_LAYER_CU;

    if( masque & ADHESIVE_LAYER_CU )
        newmasque |= ADHESIVE_LAYER_CMP;
    if( masque & ADHESIVE_LAYER_CMP )
        newmasque |= ADHESIVE_LAYER_CU;

    return newmasque;
}


/**
 * Function Move (virtual)
 * move this object.
 * @param const wxPoint& aMoveVector - the move vector for this object.
 */
void MODULE::Move(const wxPoint& aMoveVector)
{
    wxPoint newpos = m_Pos + aMoveVector;
    SetPosition( newpos );
}

/**
 * Function Rotate
 * Rotate this object.
 * @param const wxPoint& aRotCentre - the rotation point.
 * @param aAngle - the rotation angle in 0.1 degree.
 */
void MODULE::Rotate(const wxPoint& aRotCentre, int aAngle)
{
    wxPoint newpos = m_Pos;
    RotatePoint( &newpos, aRotCentre, aAngle );
    SetPosition( newpos );
    SetOrientation( m_Orient + aAngle );
}

/**
 * Function Flip
 * Flip this object, i.e. change the board side for this object
 * @param const wxPoint& aCentre - the rotation point.
 */
void MODULE::Flip(const wxPoint& aCentre )
{
    D_PAD*          pt_pad;
    TEXTE_MODULE*   pt_texte;
    EDGE_MODULE*    pt_edgmod;
    EDA_BaseStruct* PtStruct;

    // Move module to its final position:
    wxPoint finalPos = m_Pos;
    finalPos.y  = aCentre.y - (finalPos.y - aCentre.y);     /// Mirror the Y position
     SetPosition(finalPos);

    /* Flip layer */
    SetLayer( ChangeSideNumLayer( GetLayer() ) );

    /* Inversion miroir de l'orientation */
    NEGATE( m_Orient );
    NORMALIZE_ANGLE_POS( m_Orient );

    /* Inversion miroir + layers des pastilles */
    pt_pad = m_Pads;
    for( ; pt_pad != NULL; pt_pad = pt_pad->Next() )
    {
        pt_pad->m_Pos.y      -= m_Pos.y;
        pt_pad->m_Pos.y       = -pt_pad->m_Pos.y;
        pt_pad->m_Pos.y      += m_Pos.y;
        NEGATE(pt_pad->m_Pos0.y);
        NEGATE(pt_pad->m_Offset.y);
        NEGATE(pt_pad->m_DeltaSize.y);
        NEGATE_AND_NORMALIZE_ANGLE_POS( pt_pad->m_Orient );

        /* flip pads layers*/
        pt_pad->m_Masque_Layer = ChangeSideMaskLayer( pt_pad->m_Masque_Layer );
    }

    /* Inversion miroir de la Reference et mise en miroir : */
    pt_texte = m_Reference;
    pt_texte->m_Pos.y -= m_Pos.y;
    pt_texte->m_Pos.y  = -pt_texte->m_Pos.y;
    pt_texte->m_Pos.y += m_Pos.y;
    NEGATE(pt_texte->m_Pos0.y);
    pt_texte->m_Mirror = false;
    NEGATE_AND_NORMALIZE_ANGLE_POS( pt_texte->m_Orient );
    pt_texte->SetLayer( GetLayer() );
    pt_texte->SetLayer( ChangeSideNumLayer( pt_texte->GetLayer() ) );

    if( GetLayer() == COPPER_LAYER_N )
        pt_texte->SetLayer( SILKSCREEN_N_CU );

    if( GetLayer() == CMP_N )
        pt_texte->SetLayer( SILKSCREEN_N_CMP );

    if( (GetLayer() == SILKSCREEN_N_CU)
       || (GetLayer() == ADHESIVE_N_CU) || (GetLayer() == COPPER_LAYER_N) )
        pt_texte->m_Mirror = true;

    /* Inversion miroir de la Valeur et mise en miroir : */
    pt_texte = m_Value;
    pt_texte->m_Pos.y -= m_Pos.y;
    NEGATE(pt_texte->m_Pos.y);
    pt_texte->m_Pos.y += m_Pos.y;
    NEGATE(pt_texte->m_Pos0.y);
    pt_texte->m_Mirror = false;
    NEGATE_AND_NORMALIZE_ANGLE_POS( pt_texte->m_Orient );
    pt_texte->SetLayer( GetLayer() );
    pt_texte->SetLayer( ChangeSideNumLayer( pt_texte->GetLayer() ) );

    if( GetLayer() == COPPER_LAYER_N )
        pt_texte->SetLayer( SILKSCREEN_N_CU );

    if( GetLayer() == CMP_N )
        pt_texte->SetLayer( SILKSCREEN_N_CMP );

    if( (GetLayer() == SILKSCREEN_N_CU)
       || (GetLayer() == ADHESIVE_N_CU) || (GetLayer() == COPPER_LAYER_N) )
        pt_texte->m_Mirror = true;

    /* Inversion miroir des dessins de l'empreinte : */
    PtStruct = m_Drawings;
    for( ; PtStruct != NULL; PtStruct = PtStruct->Next() )
    {
        switch( PtStruct->Type() )
        {
        case TYPE_EDGE_MODULE:
            pt_edgmod = (EDGE_MODULE*) PtStruct;
            pt_edgmod->m_Start.y -= m_Pos.y;
            pt_edgmod->m_Start.y  = -pt_edgmod->m_Start.y;
            pt_edgmod->m_Start.y += m_Pos.y;
            pt_edgmod->m_End.y   -= m_Pos.y;
            pt_edgmod->m_End.y    = -pt_edgmod->m_End.y;
            pt_edgmod->m_End.y   += m_Pos.y;
            /* inversion des coords locales */
            NEGATE(pt_edgmod->m_Start0.y);
            NEGATE(pt_edgmod->m_End0.y);
            if( pt_edgmod->m_Shape == S_ARC )
            {
                NEGATE(pt_edgmod->m_Angle);
            }

            pt_edgmod->SetLayer( ChangeSideNumLayer( pt_edgmod->GetLayer() ) );
            break;

        case TYPE_TEXTE_MODULE:
            /* Inversion miroir de la position et mise en miroir : */
            pt_texte = (TEXTE_MODULE*) PtStruct;
            pt_texte->m_Pos.y -= m_Pos.y;
            pt_texte->m_Pos.y  = -pt_texte->m_Pos.y;
            pt_texte->m_Pos.y += m_Pos.y;
            NEGATE(pt_texte->m_Pos0.y);
            pt_texte->m_Mirror = false;
            NEGATE_AND_NORMALIZE_ANGLE_POS( pt_texte->m_Orient );

            pt_texte->SetLayer( GetLayer() );
            pt_texte->SetLayer( ChangeSideNumLayer( pt_texte->GetLayer() ) );

            if( GetLayer() == COPPER_LAYER_N )
                pt_texte->SetLayer( SILKSCREEN_N_CU );

            if( GetLayer() == CMP_N )
                pt_texte->SetLayer( SILKSCREEN_N_CMP );

            if(  GetLayer() == SILKSCREEN_N_CU
                 || GetLayer() == ADHESIVE_N_CU
                 || GetLayer() == COPPER_LAYER_N )
            {
                pt_texte->m_Mirror = true;
            }

            break;

        default:
            wxMessageBox( wxT( "MODULE::Flip() error: Unknown Draw Type" ) );
            break;
        }
    }

    /* calcul du rectangle d'encadrement */
    Set_Rectangle_Encadrement();
}


/*************************************************/
void MODULE::SetPosition( const wxPoint& newpos )
/*************************************************/

// replace le module en position newpos
{
    int deltaX = newpos.x - m_Pos.x;
    int deltaY = newpos.y - m_Pos.y;

    /* deplacement de l'ancre */
    m_Pos.x += deltaX;
    m_Pos.y += deltaY;

    /* deplacement de la reference */
    m_Reference->m_Pos.x += deltaX;
    m_Reference->m_Pos.y += deltaY;

    /* deplacement de la Valeur */
    m_Value->m_Pos.x += deltaX;
    m_Value->m_Pos.y += deltaY;

    /* deplacement des pastilles */
    for( D_PAD* pad = m_Pads;  pad;  pad = pad->Next() )
    {
        pad->m_Pos.x += deltaX;
        pad->m_Pos.y += deltaY;
    }

    /* deplacement des dessins de l'empreinte : */
    EDA_BaseStruct* PtStruct = m_Drawings;
    for( ; PtStruct != NULL; PtStruct = PtStruct->Next() )
    {
        switch( PtStruct->Type() )
        {
        case TYPE_EDGE_MODULE:
        {
            EDGE_MODULE* pt_edgmod = (EDGE_MODULE*) PtStruct;
            pt_edgmod->SetDrawCoord();
            break;
        }

        case TYPE_TEXTE_MODULE:
        {
            TEXTE_MODULE* pt_texte = (TEXTE_MODULE*) PtStruct;
            pt_texte->m_Pos.x += deltaX;
            pt_texte->m_Pos.y += deltaY;
            break;
        }

        default:
            wxMessageBox( wxT( "Type Draw Indefini" ) ); break;
        }
    }

    Set_Rectangle_Encadrement();
}


/*********************************************/
void MODULE::SetOrientation( int newangle )
/*********************************************/

/* Tourne de newangle (en 0.1 degres) le module
 */
{
    int px, py;

    newangle -= m_Orient;       // = delta de rotation

    m_Orient += newangle;
    NORMALIZE_ANGLE_POS( m_Orient );

    /* deplacement et rotation des pastilles */
    for( D_PAD* pad = m_Pads;  pad;  pad = pad->Next() )
    {
        px = pad->m_Pos0.x;
        py = pad->m_Pos0.y;

        pad->m_Orient += newangle; /* change m_Orientation */
        NORMALIZE_ANGLE_POS( pad->m_Orient );

        RotatePoint( &px, &py, (int) m_Orient );
        pad->m_Pos.x = m_Pos.x + px;
        pad->m_Pos.y = m_Pos.y + py;
    }

    /* mise a jour de la reference et de la valeur*/
    m_Reference->SetDrawCoord();
    m_Value->SetDrawCoord();

    /* deplacement des contours et textes de l'empreinte : */
    for( BOARD_ITEM* item = m_Drawings;  item;  item = item->Next() )
    {
        if( item->Type() == TYPE_EDGE_MODULE )
        {
            EDGE_MODULE* pt_edgmod = (EDGE_MODULE*) item;
            pt_edgmod->SetDrawCoord();
        }

        if( item->Type() == TYPE_TEXTE_MODULE )
        {
            /* deplacement des inscriptions : */
            TEXTE_MODULE* pt_texte = (TEXTE_MODULE*) item;
            pt_texte->SetDrawCoord();
        }
    }

    /* Recalcul du rectangle d'encadrement */
    Set_Rectangle_Encadrement();
}

#pragma once

class CContextHelpMappings
{
public:
    CContextHelpMappings()
    {
        // Initialize the structures.
    
        // Mappings for PPW (Welcome Page).
        m_MapPPW[0].nControlID = IDC_WP_STATIC_GROUPBOX;
        m_MapPPW[0].nStringHelpID = IDH_WP_STATIC_GROUPBOX;
        m_MapPPW[1].nControlID = IDC_WP_RADIO;
        m_MapPPW[1].nStringHelpID = IDH_WP_RADIO;
        m_MapPPW[2].nControlID = IDC_WP_RADIO2;
        m_MapPPW[2].nStringHelpID = IDH_WP_RADIO2;
        m_MapPPW[3].nControlID = IDC_WP_RADIO3;
        m_MapPPW[3].nStringHelpID = IDH_WP_RADIO3;
        m_MapPPW[4].nControlID = IDC_WP_PROGRESS;
        m_MapPPW[4].nStringHelpID = IDH_WP_PROGRESS;
        m_MapPPW[5].nControlID = IDC_WP_STATIC2;
        m_MapPPW[5].nStringHelpID = IDH_WP_PROGRESS;
        m_MapPPW[6].nControlID = IDC_WP_CHECKBOX;
        m_MapPPW[6].nStringHelpID = IDH_WP_CHECKBOX;

        // Mappings for PPA (Activation Page).
        m_MapPPA[0].nControlID = IDC_AP_STATIC;
        m_MapPPA[0].nStringHelpID = IDH_AP_EMAIL;
        m_MapPPA[1].nControlID = IDC_AP_EMAIL;
        m_MapPPA[1].nStringHelpID = IDH_AP_EMAIL;
        m_MapPPA[2].nControlID = IDC_AP_STATIC2;
        m_MapPPA[2].nStringHelpID = IDH_AP_ACTIVATION_CODE;
        m_MapPPA[3].nControlID = IDC_AP_ACTIVATION_CODE;
        m_MapPPA[3].nStringHelpID = IDH_AP_ACTIVATION_CODE;
        m_MapPPA[4].nControlID = IDC_AP_STATIC3;
        m_MapPPA[4].nStringHelpID = IDH_AP_STATIC4;
        m_MapPPA[5].nControlID = IDC_AP_STATIC4;
        m_MapPPA[5].nStringHelpID = IDH_AP_STATIC4;

        // Mappings for PP0.
        m_MapPP0[0].nControlID = IDC_PP0_STATIC;
        m_MapPP0[0].nStringHelpID = IDH_PP0_COMBOBOX;
        
        m_MapPP0[1].nControlID = IDC_PP0_COMBOBOX;
        m_MapPP0[1].nStringHelpID = IDH_PP0_COMBOBOX;

        m_MapPP0[2].nControlID = IDC_PP0_STATIC2;
        m_MapPP0[2].nStringHelpID = IDH_PP0_COMBOBOX2;
        
        m_MapPP0[3].nControlID = IDC_PP0_COMBOBOX2;
        m_MapPP0[3].nStringHelpID = IDH_PP0_COMBOBOX2;

        m_MapPP0[4].nControlID = IDC_PP0_STATIC3;
        m_MapPP0[4].nStringHelpID = IDH_PP0_EDIT;
        
        m_MapPP0[5].nControlID = IDC_PP0_EDIT;
        m_MapPP0[5].nStringHelpID = IDH_PP0_EDIT;

        m_MapPP0[6].nControlID = IDC_PP0_BUTTON1;
        m_MapPP0[6].nStringHelpID = IDH_PP0_BUTTON1;

        m_MapPP0[7].nControlID = IDC_PP0_STATIC_GROUPBOX;
        m_MapPP0[7].nStringHelpID = IDH_PP0_STATIC_GROUPBOX;
        
        m_MapPP0[8].nControlID = IDC_PP0_RADIO1;
        m_MapPP0[8].nStringHelpID = IDH_PP0_RADIO1;

        m_MapPP0[9].nControlID = IDC_PP0_RADIO2;
        m_MapPP0[9].nStringHelpID = IDH_PP0_RADIO2;

        m_MapPP0[10].nControlID = IDC_PP0_RADIO3;
        m_MapPP0[10].nStringHelpID = IDH_PP0_RADIO3;

        // Wizard's internal ID for Help button, which is not enlisted in 
        // Resource.h.
        m_MapPP0[11].nControlID = 12323; 
        m_MapPP0[11].nStringHelpID = IDH_PP0_BACK;

        m_MapPP0[12].nControlID = 12324; 
        m_MapPP0[12].nStringHelpID = IDH_PP0_NEXT;

        m_MapPP0[13].nControlID = 2; 
        m_MapPP0[13].nStringHelpID = IDH_PP0_CANCEL;

        m_MapPP0[14].nControlID = 9; 
        m_MapPP0[14].nStringHelpID = IDH_PP0_HELP;

        m_MapPP0[15].nControlID = 12325; 
        m_MapPP0[15].nStringHelpID = IDH_PP0_FINISH;

        // Mappings for PP1.
        m_MapPP1[0].nControlID = IDC_PP1_LISTVIEW;
        m_MapPP1[0].nStringHelpID = IDH_PP1_LISTVIEW;

        m_MapPP1[1].nControlID = IDC_PP1_BUTTON_ADD;
        m_MapPP1[1].nStringHelpID = IDH_PP1_BUTTON_ADD;

        m_MapPP1[2].nControlID = IDC_PP1_BUTTON_DELETE;
        m_MapPP1[2].nStringHelpID = IDH_PP1_BUTTON_DELETE;

        m_MapPP1[3].nControlID = IDC_PP1_BUTTON_MODIFY;
        m_MapPP1[3].nStringHelpID = IDH_PP1_BUTTON_MODIFY;

        // Mappings for PP2.
        m_MapPP2[0].nControlID = IDC_PP2_STATIC_GROUPBOX;
        m_MapPP2[0].nStringHelpID = IDH_PP2_STATIC_GROUPBOX;

        m_MapPP2[1].nControlID = IDC_PP2_STATIC;
        m_MapPP2[1].nStringHelpID = IDH_PP2_EDIT_TITLE;
        m_MapPP2[2].nControlID = IDC_PP2_EDIT_TITLE;
        m_MapPP2[2].nStringHelpID = IDH_PP2_EDIT_TITLE;

        m_MapPP2[3].nControlID = IDC_PP2_STATIC2;
        m_MapPP2[3].nStringHelpID = IDH_PP2_EDIT_NAME;
        m_MapPP2[4].nControlID = IDC_PP2_STATIC3;
        m_MapPP2[4].nStringHelpID = IDH_PP2_EDIT_NAME;
        m_MapPP2[5].nControlID = IDC_PP2_EDIT_NAME;
        m_MapPP2[5].nStringHelpID = IDH_PP2_EDIT_NAME;

        m_MapPP2[6].nControlID = IDC_PP2_STATIC4;
        m_MapPP2[6].nStringHelpID = IDH_PP2_STATIC4;

        m_MapPP2[7].nControlID = IDC_PP2_STATIC_GROUPBOX2;
        m_MapPP2[7].nStringHelpID = IDH_PP2_STATIC_GROUPBOX2;

        m_MapPP2[8].nControlID = IDC_PP2_STATIC5;
        m_MapPP2[8].nStringHelpID = IDH_PP2_EDIT_SETUP_NAME;
        m_MapPP2[9].nControlID = IDC_PP2_EDIT_SETUP_NAME;
        m_MapPP2[9].nStringHelpID = IDH_PP2_EDIT_SETUP_NAME;

        m_MapPP2[10].nControlID = IDC_PP2_STATIC6;
        m_MapPP2[10].nStringHelpID = IDH_PP2_STATIC6;

        m_MapPP2[11].nControlID = IDC_PP2_STATIC7;
        m_MapPP2[11].nStringHelpID = IDH_PP2_EDIT_LOCATION;
        m_MapPP2[12].nControlID = IDC_PP2_EDIT_LOCATION;
        m_MapPP2[12].nStringHelpID = IDH_PP2_EDIT_LOCATION;

        m_MapPP2[13].nControlID = IDC_PP2_BUTTON_CHANGE;
        m_MapPP2[13].nStringHelpID = IDH_PP2_BUTTON_CHANGE;

        m_MapPP2[14].nControlID = IDC_PP2_CHECKBOX;
        m_MapPP2[14].nStringHelpID = IDH_PP2_CHECKBOX;

        m_MapPP2[15].nControlID = IDC_PP2_BUTTON_MORE_PARAMS;
        m_MapPP2[15].nStringHelpID = IDH_PP2_BUTTON_MORE_PARAMS;

        m_MapPP2[16].nControlID = IDC_PP2_PROGRESSBAR;
        m_MapPP2[16].nStringHelpID = IDH_PP2_PROGRESSBAR;

        // Mappings for Optional Settings dialog.
        m_MapOS[0].nControlID = IDC_OS_LISTVIEW;
        m_MapOS[0].nStringHelpID = IDH_OS_LISTVIEW;

        m_MapOS[1].nControlID = IDC_OS_STATIC;
        m_MapOS[1].nStringHelpID = IDH_OS_STATIC_MESSAGE;

        m_MapOS[2].nControlID = IDC_OS_STATIC_MESSAGE;
        m_MapOS[2].nStringHelpID = IDH_OS_STATIC_MESSAGE;
        
        // Mappings for Picture Deatails dialog.
        m_MapPD[0].nControlID = IDC_STATIC_TITLE;
        m_MapPD[0].nStringHelpID = IDH_EDIT_TITLE;
        m_MapPD[1].nControlID = IDC_EDIT_TITLE;
        m_MapPD[1].nStringHelpID = IDH_EDIT_TITLE;

        m_MapPD[2].nControlID = IDC_PH_PREVIEW_WINDOW;
        m_MapPD[2].nStringHelpID = IDH_PH_PREVIEW_WINDOW;

        m_MapPD[3].nControlID = IDC_PH_BUTTON_CHANGE_PICTURE;
        m_MapPD[3].nStringHelpID = IDH_PH_BUTTON_CHANGE_PICTURE;
        
        m_MapPD[4].nControlID = IDC_PH_LIST;
        m_MapPD[4].nStringHelpID = IDH_PH_LIST;

        m_MapPD[5].nControlID = IDC_PH_BUTTON_MOVEUP;
        m_MapPD[5].nStringHelpID = IDH_PH_BUTTON_MOVEUP;

        m_MapPD[6].nControlID = IDC_PH_BUTTON_MOVEDOWN;
        m_MapPD[6].nStringHelpID = IDH_PH_BUTTON_MOVEDOWN;

        m_MapPD[7].nControlID = IDC_PH_BUTTON_SAVE;
        m_MapPD[7].nStringHelpID = IDH_PH_BUTTON_SAVE;

        m_MapPD[8].nControlID = IDC_PH_BUTTON_CLOSE;
        m_MapPD[8].nStringHelpID = IDH_PH_BUTTON_CLOSE;

        m_MapPD[9].nControlID = IDC_PH_BUTTON_SAVECLOSE;
        m_MapPD[9].nStringHelpID = IDH_PH_BUTTON_SAVECLOSE;

        m_MapPD[10].nControlID = IDC_PH_BUTTON_RESIZE;
        m_MapPD[10].nStringHelpID = IDH_PH_BUTTON_RESIZE;

        m_MapPD[11].nControlID = IDC_PH_STATIC_GROUPLINE;
        m_MapPD[11].nStringHelpID = IDH_PH_STATIC_GROUPLINE;

        m_MapPD[12].nControlID = IDC_PH_STATIC_YEAR;
        m_MapPD[12].nStringHelpID = IDH_PH_EDIT_XML_YEAR;
        m_MapPD[13].nControlID = IDC_PH_EDIT_XML_YEAR;
        m_MapPD[13].nStringHelpID = IDH_PH_EDIT_XML_YEAR;

        m_MapPD[14].nControlID = IDC_PH_STATIC_MAKE;
        m_MapPD[14].nStringHelpID = IDH_PH_EDIT_XML_MAKE;
        m_MapPD[15].nControlID = IDC_PH_EDIT_XML_MAKE;
        m_MapPD[15].nStringHelpID = IDH_PH_EDIT_XML_MAKE;
        
        m_MapPD[16].nControlID = IDC_PH_STATIC_MODEL;
        m_MapPD[16].nStringHelpID = IDH_PH_EDIT_XML_MODEL;
        m_MapPD[17].nControlID = IDC_PH_EDIT_XML_MODEL;
        m_MapPD[17].nStringHelpID = IDH_PH_EDIT_XML_MODEL;

        m_MapPD[18].nControlID = IDC_PH_STATIC_SUBMODEL;
        m_MapPD[18].nStringHelpID = IDH_PH_EDIT_XML_SUBMODEL;
        m_MapPD[19].nControlID = IDC_PH_EDIT_XML_SUBMODEL;
        m_MapPD[19].nStringHelpID = IDH_PH_EDIT_XML_SUBMODEL;

        m_MapPD[20].nControlID = IDC_PH_STATIC_TYPE;
        m_MapPD[20].nStringHelpID = IDH_PH_EDIT_XML_TYPE;
        m_MapPD[21].nControlID = IDC_PH_EDIT_XML_TYPE;
        m_MapPD[21].nStringHelpID = IDH_PH_EDIT_XML_TYPE;

        m_MapPD[22].nControlID = IDC_PH_STATIC_PRICE;
        m_MapPD[22].nStringHelpID = IDH_PH_EDIT_XML_PRICE;
        m_MapPD[23].nControlID = IDC_PH_EDIT_XML_PRICE;
        m_MapPD[23].nStringHelpID = IDH_PH_EDIT_XML_PRICE;

        m_MapPD[24].nControlID = IDC_PH_STATIC_GROUPLINE2;
        m_MapPD[24].nStringHelpID = IDH_PH_STATIC_GROUPLINE2;

        m_MapPD[25].nControlID = IDC_PH_STATIC_POWER;
        m_MapPD[25].nStringHelpID = IDH_PH_EDIT_XML_POWER;
        m_MapPD[26].nControlID = IDC_PH_EDIT_XML_POWER;
        m_MapPD[26].nStringHelpID = IDH_PH_EDIT_XML_POWER;

        m_MapPD[27].nControlID = IDC_PH_STATIC_FUEL;
        m_MapPD[27].nStringHelpID = IDH_PH_EDIT_XML_FUEL;
        m_MapPD[28].nControlID = IDC_PH_EDIT_XML_FUEL;
        m_MapPD[28].nStringHelpID = IDH_PH_EDIT_XML_FUEL;

        m_MapPD[29].nControlID = IDC_PH_STATIC_GROUPLINE3;
        m_MapPD[29].nStringHelpID = IDH_PH_STATIC_GROUPLINE3;

        m_MapPD[30].nControlID = IDC_PH_STATIC_LENGTH;
        m_MapPD[30].nStringHelpID = IDH_PH_EDIT_XML_LENGTH;
        m_MapPD[31].nControlID = IDC_PH_EDIT_XML_LENGTH;
        m_MapPD[31].nStringHelpID = IDH_PH_EDIT_XML_LENGTH;

        m_MapPD[32].nControlID = IDC_PH_STATIC_WIDTH;
        m_MapPD[32].nStringHelpID = IDH_PH_EDIT_XML_WIDTH;
        m_MapPD[33].nControlID = IDC_PH_EDIT_XML_WIDTH;
        m_MapPD[33].nStringHelpID = IDH_PH_EDIT_XML_WIDTH;
        
        m_MapPD[34].nControlID = IDC_PH_STATIC_HEIGHT;
        m_MapPD[34].nStringHelpID = IDH_PH_EDIT_XML_HEIGHT;
        m_MapPD[35].nControlID = IDC_PH_EDIT_XML_HEIGHT;
        m_MapPD[35].nStringHelpID = IDH_PH_EDIT_XML_HEIGHT;

        m_MapPD[36].nControlID = IDC_PH_STATIC_CLEARANCE;
        m_MapPD[36].nStringHelpID = IDH_PH_EDIT_XML_CLEARANCE;
        m_MapPD[37].nControlID = IDC_PH_EDIT_XML_CLEARANCE;
        m_MapPD[37].nStringHelpID = IDH_PH_EDIT_XML_CLEARANCE;

        m_MapPD[38].nControlID = IDC_PH_STATIC_WEIGHT;
        m_MapPD[38].nStringHelpID = IDH_PH_EDIT_XML_WEIGHT;
        m_MapPD[39].nControlID = IDC_PH_EDIT_XML_WEIGHT;
        m_MapPD[39].nStringHelpID = IDH_PH_EDIT_XML_WEIGHT;

        m_MapPD[40].nControlID = IDC_PH_STATIC_GROUPLINE4;
        m_MapPD[40].nStringHelpID = IDH_PH_STATIC_GROUPLINE4;

        m_MapPD[41].nControlID = IDC_PH_STATIC_TANK_VOLUME;
        m_MapPD[41].nStringHelpID = IDH_PH_EDIT_XML_TANK_VOLUME;
        m_MapPD[42].nControlID = IDC_PH_EDIT_XML_TANK_VOLUME;
        m_MapPD[42].nStringHelpID = IDH_PH_EDIT_XML_TANK_VOLUME;

        m_MapPD[43].nControlID = IDC_PH_STATIC_COUNTRY;
        m_MapPD[43].nStringHelpID = IDH_PH_EDIT_XML_COUNTRY;
        m_MapPD[44].nControlID = IDC_PH_EDIT_XML_COUNTRY;
        m_MapPD[44].nStringHelpID = IDH_PH_EDIT_XML_COUNTRY;
    }
        
    // For some reason, unknown to me, if m_nMapSizePPW .. m_nMapSizePD are
    // initialized in the constructor (without "const static" below), some of 
    // the values of the variables store wrong values (can be zero or arbitrary
    // high). Adding "const static" helps initialize outside the constructor, 
    // but inside the class and keeps initialization in the one place. LL only
    // const static data members can be initialized inside the class (since 
    // my intention is to have CContextHelpMappings in single header file, this
    // is a tradeoff).
    const static short m_nMapSizePPW = 7;
    const static short m_nMapSizePPA = 6;
    const static short m_nMapSizePP0 = 16;
    const static short m_nMapSizePP1 = 4;
    const static short m_nMapSizePP2 = 17;
    const static short m_nMapSizeOS = 3;
    const static short m_nMapSizePD = 45;

    struct ContextHelpMap
    {
        DWORD nControlID; 
        DWORD nStringHelpID;
    } m_MapPPW[m_nMapSizePPW], m_MapPPA[m_nMapSizePPA], 
        m_MapPP0[m_nMapSizePP0], m_MapPP1[m_nMapSizePP1], 
          m_MapPP2[m_nMapSizePP2], m_MapOS[m_nMapSizeOS], m_MapPD[m_nMapSizePD]; 
};
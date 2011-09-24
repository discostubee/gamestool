#ifndef USERPROGRAM_EXAMPLE_HPP
#define USERPROGRAM_EXAMPLE_HPP

#ifdef __APPLE__
	#include <gt_OSX/OSX_fileIO.hpp>
#elif defined LINUX
    #include <gt_linux/linux_fileIO.hpp>
#elif defined WIN32
    #include <gt_win7/win_fileIO.hpp>
#endif

#include <gt_base/anchor.hpp>
#include <gt_base/runList.hpp>
#include <gt_base/textFig.hpp>

namespace showoff{

	//- See if we can load the previous save.
	inline void load(gt::ptrFig aFile){
		using namespace gt;

		try{
			cContext fake;
			ptrLead loadAnk(new cLead(cFigment::xLoad, &fake));
			ptrLead readFile(new cLead(cBase_fileIO::xRead, &fake));
			ptrFig anchor = gWorld.get()->makeFig( getHash<cAnchor>() );

			tPlug<cByteBuffer> buff;
			readFile->add(&buff, cBase_fileIO::xPT_saveData, &fake);
			aFile->jack(readFile, &fake); //read the entire file.

			loadAnk->add( &buff, cFigment::xPT_saveData, &fake );
			anchor->jack(loadAnk, &fake);

			gWorld.get()->setRoot(anchor);

		}catch(excep::base_error &e){
			WARN(e);
		}
	}

	inline void save(gt::ptrFig aFile){
		using namespace gt;

		cContext fake;

		//- Lets setup a really basic program, using example instances
		tPlug<ptrFig> anchorExam( gWorld.get()->makeFig( getHash<cAnchor>() ) );
		tPlug<ptrFig> runlistExam( gWorld.get()->makeFig( getHash<cRunList>() ) );
		tPlug<ptrFig> textExam( gWorld.get()->makeFig( getHash<cTextFig>() ) );
		tPlug<ptrFig> endProgram( gWorld.get()->makeFig( getHash<cWorldShutoff>() ) );

		{
			ptrLead setText(new cLead(cTextFig::xSetText, &fake));
			tPlug<dStr> text;

			text = "I am error ;)";
			setText->add(&text, cTextFig::xPT_text, &fake);
			textExam.mD->jack(setText, &fake);
		}

		{
			ptrLead addToList = gWorld.get()->makeLead(getHash<cRunList>(), makeHash("add"), &fake);

			addToList->addToPile(&textExam, &fake);
			addToList->addToPile(&endProgram, &fake);
			runlistExam.mD->jack(addToList, &fake);
		}

		//- Now we make the run list the root of the anchor.
		{
			ptrLead setRoot = gWorld.get()->makeLead(getHash<cAnchor>(), makeHash("set root"), &fake);

			setRoot->add(&runlistExam, cAnchor::xPT_root, &fake);
			anchorExam.mD->jack(setRoot, &fake);
		}

		//- Save the anchor
		{
			ptrLead saveAnchor(new cLead(cFigment::xSave, &fake));
			ptrLead writeFile(new cLead(cBase_fileIO::xWrite, &fake));

			tPlug<cByteBuffer> buff;
			saveAnchor->add(&buff, cFigment::xPT_saveData, &fake);
			anchorExam.mD->jack(saveAnchor, &fake);

			writeFile->add( &buff, cBase_fileIO::xPT_saveData, &fake );
			aFile->jack(writeFile, &fake);
		}
	}

	inline void programming(){

		using namespace gt;

		cContext fake;

		tOutline<cFigment>::draft();
		tOutline<cWorldShutoff>::draft();
		tOutline<cRunList>::draft();
		tOutline<cTextFig>::draft();
		tOutline<cAnchor>::draft();
		tOutline<cBase_fileIO>::draft();

#ifdef __APPLE__
		tOutline<cOSX_fileIO>::draft();
#elif LINUX
		tOutline<cLinux_fileIO>::draft();
#elif WIN32
		tOutline<cWin_fileIO>::draft();
#endif


		DBUG_LO("Showing off user programming.");

		{
			ptrFig file = gWorld.get()->makeFig( getHash<cBase_fileIO>() );
			ptrLead setPath(new cLead(cBase_fileIO::xSetPath, &fake));
			tPlug<std::string> path( "testfile" );
			setPath->add(&path, cBase_fileIO::xPT_filePath, &fake);
			file->jack(setPath, &fake);
			showoff::save(file);
		}
		{
			ptrFig file = gWorld.get()->makeFig( getHash<cBase_fileIO>() );
			ptrLead setPath(new cLead(cBase_fileIO::xSetPath, &fake));
			tPlug<std::string> path( "testfile" );
			setPath->add(&path, cBase_fileIO::xPT_filePath, &fake);
			file->jack(setPath, &fake);
			showoff::load(file);
		}
		gWorld.get()->loop();
		gWorld.get()->setRoot(gWorld.get()->getEmptyFig());
	}
}

#endif

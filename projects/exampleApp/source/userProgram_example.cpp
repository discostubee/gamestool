#ifndef USERPROGRAM_EXAMPLE_HPP
#define USERPROGRAM_EXAMPLE_HPP

#if defined LINUX
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
			ptrLead loadAnk(new cLead(cFigment::xLoad));
			ptrLead readFile(new cLead(cBase_fileIO::xRead));
			ptrFig anchor = gWorld->makeFig( getHash<cAnchor>() );

			aFile->jack(readFile); //read the entire file.

			loadAnk->add(
				readFile->getD(cBase_fileIO::xPT_buffer),
				cFigment::xPT_buffer
			);
			anchor->jack(loadAnk);

			gWorld->setRoot(anchor);

		}catch(excep::base_error &e){
			WARN(e);
		}
	}

	inline void save(gt::ptrFig aFile){
		using namespace gt;

		//- Lets setup a really basic program, using example instances
		cPlug<ptrFig> anchorExam( gWorld->makeFig( getHash<cAnchor>() ) );
		cPlug<ptrFig> runlistExam( gWorld->makeFig( getHash<cRunList>() ) );
		cPlug<ptrFig> textExam( gWorld->makeFig( getHash<cTextFig>() ) );
		cPlug<ptrFig> endProgram( gWorld->makeFig( getHash<cWorldShutoff>() ) );

		{
			ptrLead setText(new cLead(cTextFig::xSetText));
			cPlug<dStr> text;

			text = "I am error ;)";
			setText->add(&text, cTextFig::xPT_text);
			textExam.mD->jack(setText);
		}

		{
			ptrLead addToList = gWorld->makeLead(getHash<cRunList>(), makeHash("add"));

			addToList->addToPile(&textExam);
			addToList->addToPile(&endProgram);
			runlistExam.mD->jack(addToList);
		}

		//- Now we make the run list the root of the anchor.
		{
			ptrLead setRoot = gWorld->makeLead(getHash<cAnchor>(), makeHash("set root"));

			setRoot->add(&runlistExam, cAnchor::xPT_root);
			anchorExam.mD->jack(setRoot);
		}

		//- Save the anchor
		{
			ptrLead saveAnchor(new cLead(cFigment::xSave));
			ptrLead writeFile(new cLead(cBase_fileIO::xWrite));

			anchorExam.mD->jack(saveAnchor);
			writeFile->add( saveAnchor->getD(cFigment::xPT_buffer), cBase_fileIO::xPT_buffer );
			aFile->jack(writeFile);
		}
	}

	inline void programming(){

		using namespace gt;

		tOutline<cFigment>::draft();
		tOutline<cWorldShutoff>::draft();
		tOutline<cRunList>::draft();
		tOutline<cTextFig>::draft();
		tOutline<cAnchor>::draft();
		tOutline<cBase_fileIO>::draft();

#ifdef LINUX
		tOutline<cLinux_fileIO>::draft();
#elif WIN32
		tOutline<cWin_fileIO>::draft();
#endif

		{
			//- Next lets setup the file for loading and saving.
			ptrFig file = gWorld->makeFig( getHash<cBase_fileIO>() );

			ptrLead setPath(new cLead(cBase_fileIO::xSetPath));
			cPlug<std::string> path( "testfile" );

			DBUG_LO("Showing off user programming.");

			setPath->addToPile(&path);
			file->jack(setPath);

			showoff::save(file);
			showoff::load(file);	

			gWorld->loop();
		}
	}
}

#endif

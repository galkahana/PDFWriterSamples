// standard library includes
#include <iostream>
#include <string>
using namespace std;
// end standard library includes

// pdfwriter library includes
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"

// end pdfwriter library includes

using namespace PDFHummus;

static const string scBasePath =  "..\\UsingText\\Materials\\";
static const string scSystemFontsPath = "C:\\windows\\fonts\\";

int main(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "UsingText.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		// Create a new page
		PDFPage* pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));

		// Create a content context for the page
		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		// basic text placement, with positioning (tm and td). and setting the font
		PDFUsedFont* arialTTF = pdfWriter.GetFontForFile(scSystemFontsPath + "arial.ttf");
		if(!arialTTF)
		{
			status = eFailure;
			break;
		}

		pageContentContext->k(0,0,0,1);

		pageContentContext->BT();
		pageContentContext->Tf(arialTTF,1);
		pageContentContext->Tm(20,0,0,20,40,822);
		pageContentContext->Tj("Text placed and scaled with Tm");
		pageContentContext->ET();				

		pageContentContext->BT();
		pageContentContext->Tf(arialTTF,20);
		pageContentContext->TD(40,800);
		pageContentContext->Tj("Text placed with TD and scaled with Tf size parameter");
		pageContentContext->ET();				

		// text with width control
		pageContentContext->BT();
		pageContentContext->Tf(arialTTF,20);
		pageContentContext->Tm(1,0,0,1,40,700);

		StringOrDoubleList stringsAndWidthsA;
		stringsAndWidthsA.push_back(StringOrDouble("AWAY sample with no spacing"));
		pageContentContext->TJ(stringsAndWidthsA);

		pageContentContext->Tm(1,0,0,1,40,678);

		StringOrDoubleList stringsAndWidthsB;
		//(A) 120 (W) 120 (A) 95 (Y again)
		stringsAndWidthsB.push_back(StringOrDouble("A"));
		stringsAndWidthsB.push_back(StringOrDouble(120));
		stringsAndWidthsB.push_back(StringOrDouble("W"));
		stringsAndWidthsB.push_back(StringOrDouble(120));
		stringsAndWidthsB.push_back(StringOrDouble("A"));
		stringsAndWidthsB.push_back(StringOrDouble(95));
		stringsAndWidthsB.push_back(StringOrDouble("Y sample with spacing"));
		pageContentContext->TJ(stringsAndWidthsB);
		pageContentContext->ET();
		
		// multline text with leading control
		pageContentContext->q();
		pageContentContext->BT();

		pageContentContext->Tm(1,0,0,1,40,600);
		pageContentContext->TL(24);
		pageContentContext->Tj("Some");
		pageContentContext->Quote("Multiline");
		pageContentContext->Quote("Text");

		pageContentContext->Tm(1,0,0,1,40,500);
		pageContentContext->TL(40);
		pageContentContext->Tj("Some");
		pageContentContext->Quote("More Multiline");
		pageContentContext->Quote("Text");

		pageContentContext->ET();
		pageContentContext->Q();

		// some text rendering sample
		pageContentContext->q();
		pageContentContext->BT();

		pageContentContext->Tm(1,0,0,1,40,400);
		pageContentContext->Tr(1);
		pageContentContext->Tj("Text with 1 rendering mode");		

		pageContentContext->ET();
		pageContentContext->Q();

		// diect glyph placement
		pageContentContext->BT();

		pageContentContext->Tm(1,0,0,1,40,300);
		pageContentContext->Tj("Text With Direct Glyph Placement: ");		

		GlyphUnicodeMappingList glyphs;
		glyphs.push_back(GlyphUnicodeMapping(61,'Z'));
		
		pageContentContext->Tj(glyphs);		

		pageContentContext->ET();

		// End content context, and write the page
		status = pdfWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(pdfPage);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
	}while(false);

	if(eSuccess == status)
		cout<<"Succeeded in creating PDF file\n";
	else
		cout<<"Failed in creating PDF file\n";

	return 0;
}
// standard library includes
#include <iostream>
#include <string>
using namespace std;
// end standard library includes

// pdfwriter library includes
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFComment.h"
#include "PDFCommentWriter.h."

// end pdfwriter library includes

static const wstring scBasePath =  L"..\\MakingComments\\Materials\\";
static const wstring scSystemFontsPath = L"C:\\windows\\fonts\\";

int main(int argc, wchar_t* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"MakingComments.pdf",ePDFVersion16);
		if(status != eSuccess)
			break;

		// Create a new page
		PDFPage* pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));

		// Create a content context for the page
		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		// just put some text
		PDFUsedFont* arialTTF = pdfWriter.GetFontForFile(scSystemFontsPath + L"arial.ttf");
		if(!arialTTF)
		{
			status = eFailure;
			break;
		}

		pageContentContext->k(0,0,0,1);
		pageContentContext->BT();
		pageContentContext->Tf(arialTTF,1);
		pageContentContext->Tm(20,0,0,20,40,822);
		pageContentContext->Tj(L"Text placed and scaled with Tm");
		pageContentContext->ET();				


		// End content context
		status = pdfWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
			break;


		// Now let's add some comments
		PDFCommentWriter commentWriter(&pdfWriter);

		PDFComment* aComment = new PDFComment();

		aComment->Text = L"a very important comment";
		aComment->CommentatorName = L"Someone";
		aComment->FrameBoundings[0] = 100;
		aComment->FrameBoundings[1] = 500;
		aComment->FrameBoundings[2] = 200;
		aComment->FrameBoundings[3] = 600;
		aComment->Color = CMYKRGBColor(255,0,0);

		status = commentWriter.AttachCommentToNextPage(aComment);
		if(status != eSuccess)
			break;

		PDFComment* bComment = new PDFComment();

		bComment->Text = L"I have nothing to say about this";
		bComment->CommentatorName = L"Someone";
		bComment->FrameBoundings[0] = 100;
		bComment->FrameBoundings[1] = 100;
		bComment->FrameBoundings[2] = 200;
		bComment->FrameBoundings[3] = 200;
		bComment->Color = CMYKRGBColor(255,0,0);

		status = commentWriter.AttachCommentToNextPage(bComment);
		if(status != eSuccess)
			break;

		PDFComment* cComment = new PDFComment();

		cComment->Text = L"yeah. me too. it's just perfect";
		cComment->CommentatorName = L"Someone Else";
		cComment->FrameBoundings[0] = 150;
		cComment->FrameBoundings[1] = 150;
		cComment->FrameBoundings[2] = 250;
		cComment->FrameBoundings[3] = 250;
		cComment->Color = CMYKRGBColor(0,255,0);
		cComment->ReplyTo = bComment;
		
		status = commentWriter.AttachCommentToNextPage(cComment);
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
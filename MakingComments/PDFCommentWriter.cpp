#include "PDFCommentWriter.h"
#include "PDFWriter.h"
#include "DictionaryContext.h"
#include "ObjectsContext.h"
#include "PDFRectangle.h"
#include "Trace.h"
#include "SafeBufferMacrosDefs.h"

using namespace PDFHummus;

PDFCommentWriter::PDFCommentWriter(PDFWriter* inPDFWriter)
{
	mPDFWriter = inPDFWriter;
	ListenOnPageWrites();
}

void PDFCommentWriter::ListenOnPageWrites()
{
	mPDFWriter->GetDocumentContext().AddDocumentContextExtender(this);
}

PDFCommentWriter::~PDFCommentWriter(void)
{
	// hope there's nothing to cleanup here... cause otherwise it means some comments were written
	// for a page that never arrived.
	if(mCommentsForNextPage.size() != 0)
		TRACE_LOG("PDFCommentWriter::~PDFCommentWriter, Exception. Has comments that were not associated with a page");

	CleanupComments();
}


EStatusCode PDFCommentWriter::OnPageWrite(
					PDFPage* inPage,
					DictionaryContext* inPageDictionaryContext,
					ObjectsContext* inPDFWriterObjectContext,
					DocumentContext* inPDFWriterDocumentContext)
{
	// write the comments as the page array of annotations
	
	if(mCommentsForNextPage.size() == 0)
		return eSuccess;

	if(inPageDictionaryContext->WriteKey("Annots") != eSuccess)
	{
		// Failed to write Annots key! there's already an annots entry. hmm. need to coordinate with another option
		TRACE_LOG("PDFCommentWriter::OnPageWrite, Exception. Annots already written for this page, can't write a new entr");
		return eFailure;
	}

	PDFCommentToObjectIDTypeMap::iterator it = mCommentsForNextPage.begin();

	inPDFWriterObjectContext->StartArray();
	for(; it != mCommentsForNextPage.end(); ++it)
		inPDFWriterObjectContext->WriteIndirectObjectReference(it->second);
	inPDFWriterObjectContext->EndArray(eTokenSeparatorEndLine);
	
	CleanupComments();
	return eSuccess;
}

void PDFCommentWriter::CleanupComments()
{
	PDFCommentToObjectIDTypeMap::iterator it = mCommentsForNextPage.begin();

	for(; it != mCommentsForNextPage.end(); ++it)
		delete it->first;
	mCommentsForNextPage.clear();
}

EStatusCode PDFCommentWriter::AttachCommentToNextPage(PDFComment* inComment)
{
	return WriteCommentsTree(inComment).first;
}

EStatusCodeAndObjectIDType PDFCommentWriter::WriteCommentsTree(PDFComment* inComment)
{
	EStatusCodeAndObjectIDType result;
	ObjectIDType repliedTo = 0;

	// if already written, return
	PDFCommentToObjectIDTypeMap::iterator it = mCommentsForNextPage.find(inComment);
	if(it != mCommentsForNextPage.end())
	{
		result.first = eSuccess;
		result.second = it->second;
		return result;
	}

	// if has a referred comment, write it first
	if(inComment->ReplyTo != NULL)
	{
		EStatusCodeAndObjectIDType repliedtoResult = WriteCommentsTree(inComment->ReplyTo);

		if(repliedtoResult.first != eSuccess)
		{
			result.first = eFailure;
			result.second = 0;
			return result;
		}
		else
			repliedTo = repliedtoResult.second;
	}

	do
	{
		ObjectsContext& objectsContext = mPDFWriter->GetObjectsContext();

		// Start new InDirect object for annotation dictionary
		result.second = objectsContext.StartNewIndirectObject();
		
		DictionaryContext* dictionaryContext = objectsContext.StartDictionary();


		// Type
		dictionaryContext->WriteKey("Type");
		dictionaryContext->WriteNameValue("Annot");

		// SubType
		dictionaryContext->WriteKey("Subtype");
		dictionaryContext->WriteNameValue("Text");

		// Rect
		dictionaryContext->WriteKey("Rect");
		dictionaryContext->WriteRectangleValue(
			PDFRectangle(inComment->FrameBoundings[0],
						 inComment->FrameBoundings[1],
						 inComment->FrameBoundings[2],
						 inComment->FrameBoundings[3]));

		// Contents 
		dictionaryContext->WriteKey("Contents");
		dictionaryContext->WriteLiteralStringValue(PDFTextString(inComment->Text).ToString());

		// C (color)
		dictionaryContext->WriteKey("C");
		objectsContext.StartArray();
		if(inComment->Color.UseCMYK)
		{
			objectsContext.WriteDouble((double)inComment->Color.CMYKComponents[0]/255);
			objectsContext.WriteDouble((double)inComment->Color.CMYKComponents[1]/255);
			objectsContext.WriteDouble((double)inComment->Color.CMYKComponents[2]/255);
			objectsContext.WriteDouble((double)inComment->Color.CMYKComponents[3]/255);
		}
		else
		{
			objectsContext.WriteDouble((double)inComment->Color.RGBComponents[0]/255);
			objectsContext.WriteDouble((double)inComment->Color.RGBComponents[1]/255);
			objectsContext.WriteDouble((double)inComment->Color.RGBComponents[2]/255);
		}
		objectsContext.EndArray(eTokenSeparatorEndLine);

		// T
		dictionaryContext->WriteKey("T");
		dictionaryContext->WriteLiteralStringValue(PDFTextString(inComment->CommentatorName).ToString());
		
		// M
		dictionaryContext->WriteKey("M");
		dictionaryContext->WriteLiteralStringValue(inComment->Time.ToString());

		if(inComment->ReplyTo != NULL)
		{
			// IRT
			dictionaryContext->WriteKey("IRT");
			dictionaryContext->WriteObjectReferenceValue(repliedTo);

			// RT (we're doing always "reply" at this point, being a reply to comment
			dictionaryContext->WriteKey("RT");
			dictionaryContext->WriteNameValue("R");
		}

		// Open (we'll have them all closed to begin with)
		dictionaryContext->WriteKey("Open");
		dictionaryContext->WriteBooleanValue(false);

		// Name
		dictionaryContext->WriteKey("Name");
		dictionaryContext->WriteNameValue("Comment");

		if(objectsContext.EndDictionary(dictionaryContext) != eSuccess)
		{
			result.first = eFailure;
			TRACE_LOG("PDFCommentWriter::WriteCommentsTree, Exception in ending comment dictionary");
			break;
		}
		objectsContext.EndIndirectObject();
	
		mCommentsForNextPage.insert(PDFCommentToObjectIDTypeMap::value_type(inComment,result.second));

		result.first = eSuccess;
	}while(false);
	
	return result;
}




#include "ServerPch.h"
#include "GameDBHandler.h"
#include "DBService.h"
#include "GameDBData.h"
#include "GlobalQueue.h"
#include "JobQueue.h"
#include "GameDBLoadBalancer.h"
#include "StrUtil.h"

void GameDBHandler::Init()
{
	DBHandler::Init();
	RegisterHandler(EDBProtocol::SGDB_ServerStart, &GameDBHandler::OnSGDBServerStart);
	RegisterHandler(EDBProtocol::SGDB_ChatRequest, &GameDBHandler::OnSGDBChatRequest);
}

bool GameDBHandler::OnSGDBServerStart(std::shared_ptr<DBData> data)
{
	GetSession();

	int serverId = 1;
	Poco::DateTime serverStartTime;
	try
	{
		/*
		USE [Game]
		GO
		SET ANSI_NULLS ON
			GO
			SET QUOTED_IDENTIFIER ON
			GO

			ALTER PROCEDURE[dbo].[spServerStart]
			@ServerID INT
			AS
			BEGIN
			SET NOCOUNT ON;

		BEGIN TRY
			DECLARE @TimeStamp DATETIMEOFFSET = SYSDATETIMEOFFSET();

		DELETE FROM[dbo].[ServerState] WHERE[ServerID] = @ServerID;

		INSERT INTO[dbo].[ServerState]([ServerID], [State], [Time])
			VALUES(@ServerID, 1, @TimeStamp);

		SELECT @TimeStamp;
		END TRY
			BEGIN CATCH
			ROLLBACK;
		INSERT[dbo].[TProcedureError] DEFAULT VALUES;
		THROW;
		END CATCH
			END;

		*/
		session << "EXEC spServerStart ?",
			use(serverId), // ���� ĳ����
			into(serverStartTime),  // Timestamp�� �ޱ�
			now;
	}
	catch (Poco::Data::ODBC::StatementException& ex)
	{
		VIEW_WRITE_ERROR("\n{}", StrUtil::ToString(ex.message().c_str()));
	}
	catch (std::exception& e)
	{
		VIEW_WRITE_ERROR("\nDB Error : {}", e.what());
	}

	//Poco::Data::Statement select(session); // SELECT �ÿ� ���, range(0,1)
	/*
	while (!select.done())
	{
		if (select.execute() > 0)
		{
		}
	}
	*/
	data->ProcessDBWorking();
	return true;
}

bool GameDBHandler::OnSGDBChatRequest(std::shared_ptr<DBData> data)
{
	GetSession();
	std::shared_ptr<spChatRequest> req = static_pointer_cast<spChatRequest>(data);
	if (req == nullptr)
		return false;

	try
	{
		/*
		USE [Game]
		GO
		SET ANSI_NULLS ON
			GO
			SET QUOTED_IDENTIFIER ON
			GO

			ALTER PROCEDURE[dbo].[spChatRequest]
			@Result INT OUTPUT,
			@WorkID INT
			AS
			BEGIN
			SET NOCOUNT ON;

		SET @Result = 1;
		--���� �ð��� ��������
			DECLARE @CurrentTime DATETIME = GETDATE();

		--WorkID ���� ���� Ȯ��
			IF EXISTS(SELECT 1 FROM[dbo].[ChatTime] WHERE WorkID = @WorkID)
			BEGIN
			-- WorkID�� �����ϸ� Count ���� �� Time ������Ʈ
			UPDATE[dbo].[ChatTime]
			SET Count = Count + 1,
			Time = @CurrentTime
			WHERE WorkID = @WorkID;
		END
			ELSE
			BEGIN
			-- WorkID�� ������ �� �� ����
			INSERT INTO ChatTime(WorkID, Time, Count)
			VALUES(@WorkID, @CurrentTime, 1);
		END

			IF @@ROWCOUNT = 0
			BEGIN
			SET @Result = -1;
		END
			ELSE
			BEGIN
			SET @Result = 0;
		END
			END;
		*/

		Poco::Data::Statement state(session);
		state << std::format("							\
			DECLARE @result INT;						\
			EXEC spChatRequest @result OUTPUT, {};		\
			SELECT @result;								\
		"
			, req->SessionID)
			, into(req->Result);
		state.execute();

		//session << "EXEC spChatRequest ?,?",
		//	out(req->Result),
		//	in(req->SessionID),
		//	now;
	}
	catch (Poco::Data::ODBC::StatementException& ex)
	{
		VIEW_WRITE_ERROR("\n{}", StrUtil::ToString(ex.message().c_str()));
	}
	catch (std::exception& e)
	{
		VIEW_WRITE_ERROR("\nDB Error : {}", e.what());
	}

	// ���� ������� ��ȯ
	data->ProcessDBWorking();
	return true;
}

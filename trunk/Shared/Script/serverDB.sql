USE [master]
--GO
--DROP DATABASE [StorageDB]
--GO
CREATE DATABASE [StorageDB]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'StorageDB', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL11.MSSQLSERVER\MSSQL\DATA\StorageDB.mdf' , SIZE = 5120KB , MAXSIZE = UNLIMITED, FILEGROWTH = 1024KB )
 LOG ON 
( NAME = N'StorageDB_log', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL11.MSSQLSERVER\MSSQL\DATA\StorageDB_log.ldf' , SIZE = 1024KB , MAXSIZE = 2048GB , FILEGROWTH = 10%)
GO
ALTER DATABASE [StorageDB] SET COMPATIBILITY_LEVEL = 110
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [StorageDB].[dbo].[sp_fulltext_database] @action = 'enable'
end
GO
ALTER DATABASE [StorageDB] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [StorageDB] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [StorageDB] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [StorageDB] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [StorageDB] SET ARITHABORT OFF 
GO
ALTER DATABASE [StorageDB] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [StorageDB] SET AUTO_CREATE_STATISTICS ON 
GO
ALTER DATABASE [StorageDB] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [StorageDB] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [StorageDB] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [StorageDB] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [StorageDB] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [StorageDB] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [StorageDB] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [StorageDB] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [StorageDB] SET  DISABLE_BROKER 
GO
ALTER DATABASE [StorageDB] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [StorageDB] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [StorageDB] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [StorageDB] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [StorageDB] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [StorageDB] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [StorageDB] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [StorageDB] SET RECOVERY FULL 
GO
ALTER DATABASE [StorageDB] SET  MULTI_USER 
GO
ALTER DATABASE [StorageDB] SET PAGE_VERIFY CHECKSUM  
GO
ALTER DATABASE [StorageDB] SET DB_CHAINING OFF 
GO
ALTER DATABASE [StorageDB] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [StorageDB] SET TARGET_RECOVERY_TIME = 0 SECONDS 

GO
USE [StorageDB]
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Files](
	[FileID] [int] IDENTITY(0,1) NOT NULL,
	[VerID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[ServerPath] [nvarchar](max) NOT NULL,
	[ClientRelativePath] [nvarchar](max) NOT NULL,
	[LastModDate] [bigint] NOT NULL,
 CONSTRAINT [PK_Files] PRIMARY KEY CLUSTERED 
(
	[FileID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Users](
	[UserID] [int] IDENTITY(0,1) NOT NULL,
	[Username] [nvarchar](50) NOT NULL,
	[Password] [nvarchar](max) NOT NULL,
	[Salt] [nvarchar](max) NOT NULL,
 CONSTRAINT [PK_User_Prova] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Versions](
	[VerID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[VerDate] [bigint] NOT NULL,
 CONSTRAINT [PK_Versions] PRIMARY KEY CLUSTERED 
(
	[VerID] ASC,
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
ALTER TABLE [dbo].[Users] ADD UNIQUE ([Username])

GO
ALTER TABLE [dbo].[Files]  WITH CHECK ADD  CONSTRAINT [FK_Files_Versions] FOREIGN KEY([VerID], [UserID])
REFERENCES [dbo].[Versions] ([VerID], [UserID])
ON UPDATE CASCADE
ON DELETE CASCADE

GO
ALTER TABLE [dbo].[Files] CHECK CONSTRAINT [FK_Files_Versions]

GO
ALTER TABLE [dbo].[Versions]  WITH CHECK ADD  CONSTRAINT [FK_Versions_Users] FOREIGN KEY([UserID])
REFERENCES [dbo].[Users] ([UserID])
ON UPDATE CASCADE
ON DELETE CASCADE

GO
ALTER TABLE [dbo].[Versions] CHECK CONSTRAINT [FK_Versions_Users]

GO
USE [master]
GO
ALTER DATABASE [StorageDB] SET  READ_WRITE 
GO
